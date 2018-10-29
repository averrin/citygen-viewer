#include <SFML/Graphics.hpp>

#include <app/drawableMap.hpp>
#include <app/utils.hpp>

DrawableMap::DrawableMap(std::shared_ptr<CityGen::Map> m) : map(m) { update(); }

std::vector<std::shared_ptr<sf::Drawable>>
DrawableMap::wall(CityGen::Primitive p, Polygon_with_holes polygon) {
  return wall(p, polygon.outer_boundary());
  }

std::vector<std::shared_ptr<sf::Drawable>>
DrawableMap::wall(CityGen::Primitive p, Polygon polygon) {

  auto mod = localScale * scale;
  std::vector<std::shared_ptr<sf::Drawable>> result;

  auto plot = std::make_shared<thor::ConcaveShape>(utils::toShape(polygon));
  plot->setScale(sf::Vector2f(mod, mod));
  // plot->setFillColor(utils::colorByName("light green"));
  plot->setFillColor(sf::Color::Transparent);
  plot->setOutlineColor(utils::colorByName("dim gray"));
  plot->setOutlineThickness(1);
  plot->setPosition(sf::Vector2f(p.x * mod, p.y * mod));
  result.push_back(plot);
  auto r = mod;
  for (auto point : polygon.container()) {
    auto site = std::make_shared<sf::CircleShape>(r);
    site->setFillColor(utils::colorByName("dim gray"));
    site->setPosition(
        sf::Vector2f(CGAL::to_double(p.x + point.x()) * mod - r / 2 - 2,
                     CGAL::to_double(p.y + point.y()) * mod - r / 2 - 2));

    result.push_back(site);
  }
  return result;
}

void DrawableMap::update() {
  if (!damaged) {
    return;
  }
  shapes.clear();
  auto mod = localScale * scale;

  for (auto a : map->areas) {
    if (a.type == CityGen::AreaType::WALLED) {
      auto aw = wall(a, a.polygon);
      shapes.insert(shapes.end(), aw.begin(), aw.end());
    } else {
      auto area =
          std::make_shared<thor::ConcaveShape>(utils::toShape(a.polygon));
      area->setScale(sf::Vector2f(mod, mod));
      area->setFillColor(utils::colorByName("light grey"));
      area->setPosition(sf::Vector2f(a.x * mod, a.y * mod));
      shapes.push_back(area);
    }
  }

  for (auto b : map->buildings) {
    auto bw = wall(b, b.plot);
    shapes.insert(shapes.end(), bw.begin(), bw.end());

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
