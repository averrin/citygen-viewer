#include <citygen/generator.hpp>

namespace CityGen {

std::shared_ptr<Map> Generator::createMap() {
  auto map = std::make_shared<Map>();
  auto padding = 30.f;

  auto w = 10; //24
  auto h = 8; //16
  for (auto i = 0; i < w; i++) {
    for (auto n = 0; n < h; n++) {

      // auto padding = gen->R(15, 20);

      auto b = buildingGenerator->randomBuilding(gen->R(1, 5));
      b.x = padding * (i + 1);
      b.y = padding * (n + 1);
      b.center = Point(b.center.x() + padding * (i + 1), b.center.y() + padding * (n + 1));
      map->buildings.push_back(b);

      /*
      auto polygon = toShape(b);
      polygon.setScale(sf::Vector2f(localScale, localScale));
      polygon.setFillColor(colorByName("ivory"));
      // polygon.setFillColor(sf::Color::Transparent);
      // polygon.setOutlineColor(sf::Color::Red);
      // polygon.setOutlineThickness(0.2);
      polygon.setPosition(sf::Vector2f(padding * (i + 1), padding * (n + 1)));
      polygon.setRotation(gen->R(0, 359));
      tex->draw(polygon);
*/
      // for (auto point : b.outer_boundary().container()) {
      //   sf::CircleShape site(2.f);
      //   site.setFillColor(sf::Color::Green);
      //   site.setPosition(sf::Vector2f(CGAL::to_double(point.x()) * localScale
      //   + padding * (i + 1) - 1,
      //                                 CGAL::to_double(point.y()) * localScale
      //                                 + padding * (n + 1) - 1));
      //   tex->draw(site);
      // }
    }
  }
  return map;
}
} // namespace CityGen
