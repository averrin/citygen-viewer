#ifndef __BUILDING_H_
#define __BUILDING_H_

#include <fmt/format.h>
#include <list>

#include <librandom/random.hpp>

#include <citygen/common.hpp>
#include <citygen/shapes.hpp>

namespace CityGen {

class Building : public Primitive {
public:
  Polygon plot;
  Point center;
};

class BuildingGenerator : public WithRandomGenerator {
    std::shared_ptr<Shapes> shapes;
public:
  BuildingGenerator(std::shared_ptr<R::Generator> g) : WithRandomGenerator(g) {
    shapes = std::make_shared<Shapes>(gen);
  }

  Building randomBuilding(int mc = 5) {
    Building building;
    Polygon_with_holes result;

    auto c = gen->R(1, mc);
    auto q = shapes->randomQuad(gen->R(0, 5), gen->R(0, 5));
    for (auto i = 0; i < c - 1; i++) {
      auto p = shapes->randomQuad(gen->R(0, 5), gen->R(0, 5));
      if (CGAL::join(p, q, result) && !result.is_unbounded() &&
          result.outer_boundary().is_simple() &&
          result.outer_boundary().area() > 0) {
        q = result.outer_boundary();
      } else {
        i--;
      }
    }
    // q = distort(q, -1.5, 1.5);
    q = Shapes::align(q);
    q = Shapes::rotate(q, Shapes::getCenter(q), gen->R(0, 180));
    building.polygon = Polygon_with_holes(q);
    building.center = Shapes::getCenter(q);

    auto plot = getPlot(q);
    auto a = q.area();
    plot = getPlot(q);
    building.plot = plot;
    return building;
  }

  Polygon getPlot(Polygon q) {
    auto a = q.area();
    auto c = Shapes::getCenter(q);
    auto plot = Shapes::center(shapes->randomPolygon(gen->R(4, 6)), c);

    auto inside = Shapes::checkInside(q, plot);
    auto n = 0;
    while (!inside) {
      if (plot.area() > a * 7) {
        plot = Shapes::center(shapes->randomPolygon(gen->R(4, 7)), c);
      } else {
        plot = Shapes::center(Shapes::scale(plot, 1.1), c);
      }
      inside = Shapes::checkInside(q, plot);
      n++;
    }
    // fmt::print("Plot iterations: {}\n", n);
    return plot;
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
