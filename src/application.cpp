#include "app/application.hpp"

#include <imgui-sfml/imgui-SFML.h>
#include <imgui.h>

#include <libcolor/libcolor.hpp>

#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <list>

typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef Kernel::Point_2 Point_2;
typedef CGAL::Polygon_2<Kernel> Polygon_2;
typedef CGAL::Polygon_with_holes_2<Kernel> Polygon_with_holes_2;
typedef std::list<Polygon_with_holes_2> Pwh_list_2;

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
  buildingGenerator = std::make_shared<BuildingGenerator>(gen->seed);

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
      scale *= 1.2;
      break;
    case sf::Keyboard::X:
      scale *= 0.8;
      break;
    case sf::Keyboard::R:
      damaged = true;
      gen->updateSeed();
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

sf::ConvexShape toShape(Polygon_2 p) {
  sf::ConvexShape polygon;
  polygon.setPointCount(p.size());
  int n = 0;
  for (auto point : p.container()) {
    polygon.setPoint(n, sf::Vector2f(CGAL::to_double(point.x()),
                                     CGAL::to_double(point.y())));
    n++;
  }
  return polygon;
}

sf::ConvexShape toShape(Polygon_with_holes_2 p) {
  return toShape(p.outer_boundary());
}

sf::Color colorByName(std::string name) {
  auto startColor = Color::fromWebName(name);
  return sf::Color(startColor.red(), startColor.green(), startColor.blue());
}

std::shared_ptr<sf::RenderTexture> Application::drawMap() {
  auto tex = std::make_shared<sf::RenderTexture>();
  tex->setSmooth(true);
  tex->create(2400, 1800);

  auto bgColor = sf::Color(23, 23, 23);
  tex->clear(bgColor);

  auto localScale = 8.f * scale;
  auto padding = 120.f * scale;

  for (auto i = 0; i < 8; i++) {
    for (auto n = 0; n < 8; n++) {

      auto b = buildingGenerator->randomBuilding();

      auto polygon = toShape(b);
      polygon.setScale(sf::Vector2f(localScale, localScale));
      polygon.setFillColor(colorByName("ivory"));
      // polygon.setFillColor(sf::Color::Transparent);
      polygon.setOutlineColor(sf::Color::Red);
      polygon.setOutlineThickness(0.2);
      polygon.setPosition(sf::Vector2f(padding * (i + 1), padding * (n + 1)));
      tex->draw(polygon);

      for (auto point : b.outer_boundary().container()) {

        sf::CircleShape site(2.f);
        site.setFillColor(sf::Color::Green);
        site.setPosition(sf::Vector2f(CGAL::to_double(point.x()) * localScale + padding * (i + 1),
                                      CGAL::to_double(point.y()) * localScale + padding * (n + 1)));
        tex->draw(site);
      }
    }
  }
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
    window->draw(sprite);

    sf::RenderTexture mTexture;
    mTexture.create(texture->getSize().x * scale, texture->getSize().y * scale);

    mTexture.clear(bgColor);
    mTexture.draw(sprite);

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
    mTexture.draw(frame);
    mTexture.display();
    sf::Sprite mSprite(mTexture.getTexture());

    mSprite.setScale(480.f / mTexture.getSize().x,
                     360.f / mTexture.getSize().y);
    ImGui::SFML::Update(*window, deltaClock.restart());
    drawMinimap(mSprite);
    ImGui::SFML::Render(*window);
    window->display();
  }
  log.info("shutdown");
  ImGui::SFML::Shutdown();
  return 0;
}
