#include <SFML/Graphics.hpp>

#include <app/drawableMap.hpp>
#include <app/utils.hpp>

DrawableMap::DrawableMap(std::shared_ptr<CityGen::Map> m) : map(m) { update(); }

void DrawableMap::update() {
    if (!damaged) {
        return;
    }
  shapes.clear();
  auto localScale = 6.f;
  auto mod = localScale * scale;
  for (auto b : map->buildings) {
    auto plot =
        std::make_shared<thor::ConcaveShape>(utils::toShape(b.plot));
    plot->setScale(sf::Vector2f(mod, mod));
    // plot->setFillColor(utils::colorByName("light green"));
    plot->setFillColor(sf::Color::Transparent);
    plot->setOutlineColor(utils::colorByName("dim gray"));
    plot->setOutlineThickness(1);
    plot->setPosition(sf::Vector2f(b.x * mod, b.y * mod));
    shapes.push_back(plot);
    auto r = mod;
    for (auto point : b.plot.container()) {
        auto site = std::make_shared<sf::CircleShape>(r);
        site->setFillColor(utils::colorByName("dim gray"));
        site->setPosition(sf::Vector2f(
                             CGAL::to_double(b.x + point.x()) * mod - r/2 - 2,
                             CGAL::to_double(b.y + point.y()) * mod - r/2 - 2));

    shapes.push_back(site);
    }

    auto polygon =
        std::make_shared<thor::ConcaveShape>(utils::toShape(b.polygon));
    polygon->setScale(sf::Vector2f(mod, mod));
    polygon->setFillColor(utils::colorByName("ivory"));
    // polygon.setFillColor(sf::Color::Transparent);
    // polygon.setOutlineColor(sf::Color::Red);
    // polygon.setOutlineThickness(0.2);
    polygon->setPosition(sf::Vector2f(b.x * mod, b.y * mod));
    shapes.push_back(polygon);

    auto site = std::make_shared<sf::CircleShape>(2.f);
    site->setFillColor(sf::Color::Green);
    site->setPosition(sf::Vector2f(b.x * mod - 1, b.y * mod - 1));
    // shapes.push_back(site);

    auto center = std::make_shared<sf::CircleShape>(2.f);
    center->setFillColor(sf::Color::Red);
    center->setPosition(sf::Vector2f(CGAL::to_double(b.center.x()) * mod - 1,
                                     CGAL::to_double(b.center.y()) * mod - 1));
    // shapes.push_back(center);

    auto bb = b.polygon.bbox();
    auto bPolygon = std::make_shared<sf::ConvexShape>();
    bPolygon->setPointCount(4);
    bPolygon->setPoint(0, sf::Vector2f(bb.xmin(), bb.ymin()));
    bPolygon->setPoint(1, sf::Vector2f(bb.xmax(), bb.ymin()));
    bPolygon->setPoint(2, sf::Vector2f(bb.xmax(), bb.ymax()));
    bPolygon->setPoint(3, sf::Vector2f(bb.xmin(), bb.ymax()));
    bPolygon->setScale(sf::Vector2f(mod, mod));
    bPolygon->setPosition(sf::Vector2f(b.x * mod, b.y * mod));
    bPolygon->setFillColor(sf::Color::Transparent);
    bPolygon->setOutlineColor(sf::Color::Red);
    bPolygon->setOutlineThickness(0.2);
    // shapes.push_back(bPolygon);

  }

    damaged = false;
}

void DrawableMap::draw(sf::RenderTarget &target,
                       sf::RenderStates states) const {
  sf::Transform t;
  t.rotate(rotation, origin.x, origin.y);
  // t.translate(target.getSize().x/2, target.getSize().y/2);
  // fmt::print("{}x{}\n", target.getSize().x, target.getSize().y);
  for (auto polygon : shapes) {
    target.draw(*polygon, t);
  }
};
