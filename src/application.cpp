#include <app/application.hpp>
#include <app/utils.hpp>

#include <imgui-sfml/imgui-SFML.h>
#include <imgui.h>

#include <Thor/Shapes/ConcaveShape.hpp>

Application::Application(std::string app_name, std::string version)
    : APP_NAME(app_name), VERSION(version) {
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  ImGui::CreateContext();

  ImGui::StyleColorsDark();
  ImGuiIO &io = ImGui::GetIO();

  // window = new sf::RenderWindow(sf::VideoMode::getDesktopMode(), APP_NAME,
  window = new sf::RenderWindow(sf::VideoMode(1600, 900), APP_NAME,
                                sf::Style::Default, settings);
  window->setVerticalSyncEnabled(true);
  window->setFramerateLimit(60);
  ImGui::SFML::Init(*window);
  fixed = window->getView();

  gen = std::make_shared<R::Generator>();
  cityGenerator = std::make_shared<CityGen::Generator>(gen->seed);
  map = cityGenerator->createMap();
  drawableMap = std::make_shared<DrawableMap>(map);

  window->resetGLStates();
}

void Application::processEvent(sf::Event event) {
  switch (event.type) {
  case sf::Event::KeyPressed:
    switch (event.key.code) {
    case sf::Keyboard::Escape:
      window->close();
      break;
    case sf::Keyboard::Right:
      fixed.move(20.f, 0.f);
      break;
    case sf::Keyboard::Left:
      fixed.move(-20.f, 0.f);
      break;
    case sf::Keyboard::Up:
      fixed.move(0.f, -20.f);
      break;
    case sf::Keyboard::Down:
      fixed.move(0.f, 20.f);
      break;
    case sf::Keyboard::Z:
      damaged = true;
      scale *= 1.2;
      break;
    case sf::Keyboard::X:
      damaged = true;
      scale *= 0.8;
      break;
    case sf::Keyboard::K:
      damaged = true;
      rotation -= 5;
      break;
    case sf::Keyboard::L:
      damaged = true;
      rotation += 5;
      break;
    case sf::Keyboard::R:
      gen->updateSeed();
      map = cityGenerator->createMap();
      drawableMap = std::make_shared<DrawableMap>(map);
      damaged = true;
      break;
    }
    break;
  case sf::Event::Closed:
    window->close();
    break;
  case sf::Event::Resized:
    fixed.setSize(event.size.width * scale, event.size.height * scale);
    // TODO: fix minimap frame
    break;
  case sf::Event::MouseButtonPressed:
    // Mouse button is pressed, get the position and set moving as active
    if (event.mouseButton.button == 0) {
      moving = true;
      oldPos = window->mapPixelToCoords(
          sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
    }
    break;
  case sf::Event::MouseButtonReleased:
    // Mouse button is released, no longer move
    if (event.mouseButton.button == 0) {
      moving = false;
    }
    break;
  case sf::Event::MouseMoved: {
    // Ignore mouse movement unless a button is pressed (see above)
    if (!moving)
      break;
    // Determine the new position in world coordinates
    const sf::Vector2f newPos = window->mapPixelToCoords(
        sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
    // Determine how the cursor has moved
    // Swap these to invert the movement direction
    const sf::Vector2f deltaPos = oldPos - newPos;

    // Move our view accordingly and update the window
    fixed.setCenter(fixed.getCenter() + deltaPos);
    window->setView(fixed);

    // Save the new position as the old one
    // We're recalculating this, since we've changed the view
    oldPos = window->mapPixelToCoords(
        sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
    break;
  }
  case sf::Event::MouseWheelScrolled:
    // Ignore the mouse wheel unless we're not moving
    if (moving)
      break;

    // Determine the scroll direction and adjust the zoom level
    // Again, you can swap these to invert the direction
    if (event.mouseWheelScroll.delta <= -1) {
      damaged = true;
      scale *= 1.1;
    } else if (event.mouseWheelScroll.delta >= 1) {
      damaged = true;
      scale *= 0.9;
    }

    break;
  }
}

void Application::drawMinimap(sf::Sprite map) {
  ImGui::Begin("Minimap");

  map.setTextureRect(sf::IntRect(0, map.getTexture()->getSize().y,
                                 map.getTexture()->getSize().x,
                                 -map.getTexture()->getSize().y));
  ImGui::Image(map);
  // ImGui::SliderInt("Count", &count, 1, 200);
  // ImGui::SliderFloat("Size", &size, 4.f, 200.f);
  ImGui::Text("\n");
  ImGui::SliderFloat("Zoom", &scale, 0.05f, 3.f);

  ImGui::End();
}

std::shared_ptr<sf::RenderTexture> Application::drawMap() {
  auto tex = std::make_shared<sf::RenderTexture>();
  tex->setSmooth(true);
  tex->create(2400 * scale, 1800 * scale);

  auto bgColor = sf::Color(23, 23, 23);
  tex->clear(bgColor);
  drawableMap->setScale(scale);
  tex->draw(*drawableMap);

  return tex;
}

int Application::serve() {
  log.info("serve");
  sf::Clock deltaClock;
  while (window->isOpen()) {
    sf::Event event;
    while (window->pollEvent(event)) {
      ImGui::SFML::ProcessEvent(event);
      processEvent(event);
    }

    window->setView(fixed);
    auto bgColor = sf::Color(23, 23, 23);
    window->clear(bgColor);

    if (damaged) {
      cache = drawMap();
      damaged = false;
    }
    auto texture = cache;
    texture->display();
    sf::Sprite sprite(texture->getTexture());
    //TODO: rotate from center
    sprite.setOrigin(sf::Vector2f(texture->getSize().x/2, texture->getSize().y/2));
    sprite.setRotation(rotation);
    sprite.move(sf::Vector2f(texture->getSize().x/2, texture->getSize().y/2));
    // sprite.setOrigin(sf::Vector2f(0, 0));
    window->draw(sprite);

    sf::RenderTexture mTexture;
    mTexture.create(texture->getSize().x, texture->getSize().y);

    mTexture.clear(bgColor);
    sf::Sprite oSprite(texture->getTexture());
    mTexture.draw(oSprite);

    sf::RectangleShape frame;
    int t = 20;
    frame.setPosition((fixed.getCenter().x - window->getSize().x / 2),
                      (fixed.getCenter().y - window->getSize().y / 2));
    frame.setSize(
        sf::Vector2f(fixed.getViewport().width * window->getSize().x,
                     fixed.getViewport().height * window->getSize().y));
    frame.setFillColor(sf::Color::Transparent);
    frame.setOutlineThickness(t);
    frame.setOutlineColor(sf::Color(200, 200, 200));
    frame.setOrigin(sf::Vector2f(frame.getSize().x/2, frame.getSize().y/2));
    frame.setRotation(-rotation);
    frame.setOrigin(sf::Vector2f(0, 0));
    mTexture.draw(frame);
    mTexture.display();
    sf::Sprite mSprite(mTexture.getTexture());

    mSprite.setScale(480.f / mTexture.getSize().x,
                     360.f / mTexture.getSize().y);
    // sprite.setOrigin(sf::Vector2f(mTexture.getSize().x/2, mTexture.getSize().y/2));
    // mSprite.setRotation(rotation);
    ImGui::SFML::Update(*window, deltaClock.restart());
    drawMinimap(mSprite);
    ImGui::SFML::Render(*window);
    window->display();
  }
  log.info("shutdown");
  ImGui::SFML::Shutdown();
  return 0;
}
