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
    }
    break;
  case sf::Event::Closed:
    window->close();
    break;
  case sf::Event::Resized:
    fixed.setSize(event.size.width * scale, event.size.height * scale);
    break;
  }
}

float value = 1.f;
int count = 25;
float size = 100.f;

void Application::drawMinimap(sf::Sprite map) {
  ImGui::Begin("Minimap");

  map.setTextureRect(sf::IntRect(0, map.getTexture()->getSize().y, map.getTexture()->getSize().x, -map.getTexture()->getSize().y));
  ImGui::Image(map);
  // ImGui::SliderInt("Count", &count, 1, 200);
  // ImGui::SliderFloat("Size", &size, 4.f, 200.f);
  ImGui::Text("\n");
  ImGui::SliderFloat("Zoom", &scale, 0.05f, 5.f);

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

std::shared_ptr<sf::RenderTexture> Application::drawMap() {
  auto tex = std::make_shared<sf::RenderTexture>();
  tex->setSmooth(true);
  tex->create(2400, 1800);

  auto bgColor = sf::Color(23, 23, 23);
  tex->clear(bgColor);

  Polygon_2 P;
  P.push_back(Point_2(0, 0));
  P.push_back(Point_2(50, 0));
  P.push_back(Point_2(35, 15));
  P.push_back(Point_2(25, 5));
  P.push_back(Point_2(15, 15));

  Polygon_2 Q;
  Q.push_back(Point_2(0, 20));
  Q.push_back(Point_2(15, 5));
  Q.push_back(Point_2(25, 15));
  Q.push_back(Point_2(35, 5));
  Q.push_back(Point_2(50, 20));

  auto localScale = 10.f * scale;
  auto padding = 100.f * scale;
  Polygon_with_holes_2 unionR;
  if (CGAL::join(P, Q, unionR)) {
    // if (! unionR.is_unbounded()) {
    auto polygon = toShape(Q);
    polygon.setScale(sf::Vector2f(localScale, localScale));
    polygon.setFillColor(sf::Color::Red);
    // polygon.setOutlineColor(sf::Color::White);
    // polygon.setOutlineThickness(0.1);
    polygon.setPosition(sf::Vector2f(padding, padding));
    tex->draw(polygon);

    polygon = toShape(P);
    polygon.setScale(sf::Vector2f(localScale, localScale));
    polygon.setFillColor(sf::Color::Blue);
    // polygon.setOutlineColor(sf::Color::Black);
    // polygon.setOutlineThickness(0.1);
    polygon.setPosition(sf::Vector2f(padding, padding));
    tex->draw(polygon);

    polygon = toShape(unionR);
    polygon.setScale(sf::Vector2f(localScale, localScale));
    polygon.setFillColor(sf::Color::White);
    polygon.setPosition(sf::Vector2f(padding, padding));
    tex->draw(polygon);

    for (auto hit = unionR.holes_begin(); hit != unionR.holes_end(); ++hit) {
      polygon = toShape(*hit);
      polygon.setScale(sf::Vector2f(localScale, localScale));
      polygon.setFillColor(bgColor);
      polygon.setPosition(sf::Vector2f(padding, padding));
      tex->draw(polygon);
    }

    Pwh_list_2                  intR;

    CGAL::intersection (P, Q, std::back_inserter(intR));
    for (auto ip : intR) {
      polygon = toShape(ip);
      polygon.setScale(sf::Vector2f(localScale, localScale));

      auto startColor = Color::fromWebName("red");
      startColor.blend(Color::fromWebName("blue"), 0.3);
      auto sfColor =
          sf::Color(startColor.red(), startColor.green(), startColor.blue());
      polygon.setFillColor(sfColor);
      // polygon.setOutlineColor(sfColor);
      // polygon.setOutlineThickness(1);
      polygon.setPosition(sf::Vector2f(padding, padding));
      tex->draw(polygon);
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

    auto texture = drawMap();
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
    frame.setSize(sf::Vector2f(
        fixed.getViewport().width * window->getSize().x,
        fixed.getViewport().height * window->getSize().y));
    frame.setFillColor(sf::Color::Transparent);
    frame.setOutlineThickness(t);
    frame.setOutlineColor(sf::Color(200, 200, 200));
    mTexture.draw(frame);
    mTexture.display();
    sf::Sprite mSprite(mTexture.getTexture());


    mSprite.setScale(480.f/mTexture.getSize().x, 360.f/mTexture.getSize().y);
    ImGui::SFML::Update(*window, deltaClock.restart());
    drawMinimap(mSprite);
    ImGui::SFML::Render(*window);
    window->display();
  }
  log.info("shutdown");
  ImGui::SFML::Shutdown();
  return 0;
}
