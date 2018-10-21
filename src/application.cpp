#include "app/application.hpp"

#include <imgui-sfml/imgui-SFML.h>
#include <imgui.h>

#include <libcolor/libcolor.hpp>

Application::Application(std::string app_name, std::string version)
    : APP_NAME(app_name), VERSION(version) {
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();

  // window = new sf::RenderWindow(sf::VideoMode::getDesktopMode(), APP_NAME,
  window = new sf::RenderWindow(sf::VideoMode(1600, 900), APP_NAME,
                                sf::Style::Default, settings);
  window->setVerticalSyncEnabled(true);
  window->setFramerateLimit(60);
  ImGui::SFML::Init(*window);
  updateMinimap();
  fixed = window->getView(); // The 'fixed' view will never change


  window->resetGLStates();
}
void Application::updateMinimap() {
  unsigned int size =
      400; // The 'minimap' view will show a smaller picture of the map
  minimap = sf::View(sf::FloatRect(
      fixed.getCenter().x, fixed.getCenter().y, static_cast<float>(size),
      static_cast<float>(window->getSize().y * size / window->getSize().x)));
  minimap.setViewport(sf::FloatRect(
      1.f - static_cast<float>(minimap.getSize().x) / window->getSize().x -
          0.02f,
      1.f - static_cast<float>(minimap.getSize().y) / window->getSize().y -
          0.02f,
      static_cast<float>(minimap.getSize().x) / window->getSize().x,
      static_cast<float>(minimap.getSize().y) / window->getSize().y));
  minimap.zoom(zoom);

}

void Application::processEvent(sf::Event event) {
  ImGui::SFML::ProcessEvent(event);
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
      fixed.zoom(1.2);
      scale *= 1.2;
      break;
    case sf::Keyboard::X:
      fixed.zoom(0.8);
      scale *= 0.8;
      break;
    }
    break;
  case sf::Event::Closed:
    window->close();
    break;
  case sf::Event::Resized:
    fixed.setSize(event.size.width * scale, event.size.height * scale);
    updateMinimap();
    break;
  }
}

float value = 1.f;
int count = 25;
float size = 100.f;

void Application::drawMainWindow() {
  ImGui::Begin(APP_NAME.c_str());
  ImGui::SliderInt("Count", &count, 1, 200);
  ImGui::SliderFloat("Size", &size, 4.f, 200.f);
  ImGui::SliderFloat("Value", &value, 0.f, 1.f);

  ImGui::End();
}

std::shared_ptr<sf::RenderTexture> drawMap() {
  auto tex = std::make_shared<sf::RenderTexture>();
  tex->create(2400, 1800);

  auto bgColor = sf::Color(23, 23, 23);
  tex->clear(bgColor);

  float padding = size / 10;
  auto yOffset = 0.f;
  for (auto y = 0; y < count; y++) {
    auto offset = 0.f;
    auto startColor = Color::fromWebName("red");
    startColor.saturation(1 - y / float(count));
    startColor.value(value);
    for (auto i = 0; i < count; i++) {
      auto sfColor =
          sf::Color(startColor.red(), startColor.green(), startColor.blue());

      sf::VertexArray quad(sf::Quads, 4);

      quad[0].position = sf::Vector2f(padding + offset, padding + yOffset);
      quad[0].color = sfColor;
      quad[1].position =
          sf::Vector2f(padding + size + offset, padding + yOffset);
      quad[1].color = sfColor;
      quad[2].position =
          sf::Vector2f(padding + size + offset, padding + size + yOffset);
      quad[2].color = sfColor;
      quad[3].position =
          sf::Vector2f(padding + offset, padding + size + yOffset);
      quad[3].color = sfColor;

      tex->draw(quad);
      offset += size + padding;
      startColor.hue(startColor.hue() + 360.f / count);
    }
    yOffset += size + padding;
  }

  return tex;
}

int Application::serve() {
  log.info("serve");
  sf::Clock deltaClock;
  while (window->isOpen()) {
    sf::Event event;
    while (window->pollEvent(event)) {
      processEvent(event);
    }

    window->setView(fixed);
    auto bgColor = sf::Color(23, 23, 23);
    window->clear(bgColor);

    auto texture = drawMap();
    texture->display();
    sf::Sprite sprite(texture->getTexture());
    window->draw(sprite);

    sf::RectangleShape frame;
    int t = 20;
    frame.setPosition(fixed.getCenter().x - window->getSize().x * scale / 2,
                      fixed.getCenter().y - window->getSize().y * scale / 2);
    frame.setSize(
        sf::Vector2f(fixed.getViewport().width * window->getSize().x * scale + t,
                     fixed.getViewport().height * window->getSize().y * scale + t));
    frame.setFillColor(sf::Color::Transparent);
    frame.setOutlineThickness(t);
    frame.setOutlineColor(sf::Color(200, 200, 200));
    texture->draw(frame);
    texture->display();
    window->setView(minimap); // Draw minimap
    window->draw(sprite);

    ImGui::SFML::Update(*window, deltaClock.restart());
    drawMainWindow();
    ImGui::SFML::Render(*window);
    window->display();
  }
  log.info("shutdown");
  ImGui::SFML::Shutdown();
  return 0;
}
