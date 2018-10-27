#ifndef __UTILS_H_
#define __UTILS_H_
#include <SFML/Graphics.hpp>
#include <Thor/Shapes/ConcaveShape.hpp>

#include <libcolor/libcolor.hpp>
using namespace LibColor;

#include <citygen/common.hpp>

class utils {
public:
  static sf::Color colorByName(std::string name) {
    auto startColor = Color::fromWebName(name);
    return sf::Color(startColor.red(), startColor.green(), startColor.blue());
  }

  static thor::ConcaveShape toShape(Polygon p) {
    thor::ConcaveShape polygon;
    polygon.setPointCount(p.size());
    int n = 0;
    for (auto point : p.container()) {
      polygon.setPoint(n, sf::Vector2f(CGAL::to_double(point.x()),
                                       CGAL::to_double(point.y())));
      n++;
    }
    return polygon;
  }

  static thor::ConcaveShape toShape(Polygon_with_holes p) {
    return toShape(p.outer_boundary());
  }
};

#endif // __UTILS_H_
