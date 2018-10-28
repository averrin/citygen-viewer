#include <citygen/generator.hpp>

namespace CityGen {

std::shared_ptr<Map> Generator::createMap(float width, float height) {
  auto map = std::make_shared<Map>();

  auto city = Area();
  city.type = AreaType::CITY;
  city.polygon = Polygon_with_holes(
    Shapes::centrate(
      Shapes::scale(
        Shapes::randomPolygon(
          gen->R(7, 12)
        ),
      30),
    Point(width / 2, height / 2)));
  map->areas.push_back(city);

  auto castle = buildingGenerator->randomBuilding(gen->R(1, 5));
  castle.x = width / 2;
  castle.y = height / 2;
  // b.center = Point(b.center.x() + padding * (i + 1), b.center.y() + padding *
  // (n + 1));
  map->buildings.push_back(castle);

  return map;
}
} // namespace CityGen
