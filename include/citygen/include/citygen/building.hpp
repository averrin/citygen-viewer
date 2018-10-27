#ifndef __BUILDING_H_
#define __BUILDING_H_

#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <fmt/format.h>
#include <list>
#include <memory>
#include <cmath>

#include <librandom/random.hpp>

#include <citygen/common.hpp>

namespace CityGen {

class Building {
public:
  float x;
  float y;
  Polygon_with_holes polygon;
  Point center;
};

class BuildingGenerator {
  std::shared_ptr<R::Generator> gen;

public:
  BuildingGenerator(int seed) {
    gen = std::make_shared<R::Generator>();
    gen->setSeed(seed);
  }
  Polygon randomQuad(float dx = 0, float dy = 0) {
    auto w = gen->R(2.f, 10.f);
    auto h = gen->R(2.f, 10.f);
    Polygon Q;

    Q.push_back(Point(dx, dy));
    Q.push_back(Point(dx + w, dy));
    Q.push_back(Point(dx + w, dy + h));
    Q.push_back(Point(dx, dy + h));

    if (!Q.is_simple()) {
      Q = randomQuad(dx, dy);
    }
    return Q;
  }

  Polygon distort(Polygon p, float min, float max) {
    Polygon d;

    for (auto point : p.container()) {
      d.push_back(Point(
                    abs(point.x() + gen->R(min, max)),
                    abs(point.y() + gen->R(min, max))
                  ));
    }

    return d;
  }

  Polygon align(Polygon p) {
    Polygon d;
    auto dx = p.bbox().xmin();
    auto dy = p.bbox().ymin();

    for (auto point : p.container()) {
      d.push_back(Point(
                    point.x() - dx,
                    point.y() - dy
                  ));
    }

    return d;

    }

  Building randomBuilding(int mc = 5) {
    Building building;
    Polygon_with_holes result;

    auto c = gen->R(1, mc);
    auto q = randomQuad(gen->R(0, 5), gen->R(0, 5));
    for (auto i = 0; i < c - 1; i++) {
      auto p = randomQuad(gen->R(0, 5), gen->R(0, 5));
      if (CGAL::join(p, q, result) && !result.is_unbounded() &&
          result.outer_boundary().is_simple() &&
          result.outer_boundary().area() > 0) {
        q = result.outer_boundary();
      } else {
        i--;
      }
    }
    // q = distort(q, -1.5, 1.5);
    building.polygon = Polygon_with_holes(align(q));

    building.center = Point(
        building.polygon.bbox().xmin() +
            (building.polygon.bbox().xmax() - building.polygon.bbox().xmin()) /
                2,
        building.polygon.bbox().ymin() +
            (building.polygon.bbox().ymax() - building.polygon.bbox().ymin()) /
                2);
    return building;
  }
};
} // namespace CityGen
/*

    Pwh_list                  intR;

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
  */

#endif // __BUILDING_H_
