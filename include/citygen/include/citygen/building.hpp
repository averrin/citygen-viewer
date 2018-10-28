#ifndef __BUILDING_H_
#define __BUILDING_H_

#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <cmath>
#include <fmt/format.h>
#include <list>
#include <memory>

#include <librandom/random.hpp>

#include <citygen/common.hpp>

namespace CityGen {

class Building {
public:
  float x;
  float y;
  Polygon_with_holes polygon;
  Polygon plot;
  Point center;
};

class BuildingGenerator {
  std::shared_ptr<R::Generator> gen;

public:
  BuildingGenerator(int seed) {
    gen = std::make_shared<R::Generator>();
    gen->setSeed(seed);
  }

  bool checkInside(Point pt, Polygon p) {
    switch (
        CGAL::bounded_side_2(p.container().begin(), p.container().end(), pt)) {
    case CGAL::ON_BOUNDED_SIDE:
      return true;
      break;
    case CGAL::ON_BOUNDARY:
      return true;
      break;
    case CGAL::ON_UNBOUNDED_SIDE:
      return false;
      break;
    }
    return false;
  }

  bool checkInside(Polygon q, Polygon p) {
    auto bb = q.bbox();
    std::vector<Point> points = {
        Point(bb.xmin(), bb.ymin()), Point(bb.xmax(), bb.ymin()),
        Point(bb.xmax(), bb.ymax()), Point(bb.xmin(), bb.ymax())};

    for (auto point : points) {
      if (!checkInside(point, p)) {
        return false;
      }
    }
    return true;
  }

  Polygon centrate(Polygon p, Point origin) {
    auto c = getCenter(p);
    auto dx = origin.x() - c.x();
    auto dy = origin.y() - c.y();
    Polygon t;
    for (auto point : p.container()) {
      t.push_back(Point(point.x() + dx, point.y() + dy));
    }
    return t;
  }

  Polygon scale(Polygon p, float mod) {
    Polygon t;
    for (auto point : p.container()) {
      t.push_back(Point(point.x() * mod, point.y() * mod));
    }
    return t;
  }

  Polygon translate(Polygon p, Point origin) {
    Polygon t;
    for (auto point : p.container()) {
      t.push_back(Point(point.x() + origin.x(), point.y() + origin.y()));
    }
    return t;
  }

  Polygon randomPolygon(int c, float origin = 5) {
    Polygon p;

    CGAL::random_convex_set_2(c, std::back_inserter(p),
                              Point_generator(origin));
    return p;
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
      d.push_back(Point(abs(point.x() + gen->R(min, max)),
                        abs(point.y() + gen->R(min, max))));
    }

    return d;
  }

  Polygon align(Polygon p) {
    Polygon d;
    auto dx = p.bbox().xmin();
    auto dy = p.bbox().ymin();

    for (auto point : p.container()) {
      d.push_back(Point(point.x() - dx, point.y() - dy));
    }

    return d;
  }

  Point rotate(Point p, Point origin, float angle) {
    auto tx = p.x() - origin.x();
    auto ty = p.y() - origin.y();
    auto rx = tx * cos(angle) - ty * sin(angle);
    auto ry = tx * sin(angle) + ty * cos(angle);
    return Point(rx + origin.x(), ry + origin.y());
  }

  Polygon rotate(Polygon p, Point origin, float angle) {
    Polygon d;
    for (auto point : p.container()) {
      d.push_back(rotate(point, origin, angle));
    }
    return d;
  }

  Point getCenter(Polygon polygon) {
    return Point(polygon.bbox().xmin() +
                     (polygon.bbox().xmax() - polygon.bbox().xmin()) / 2,
                 polygon.bbox().ymin() +
                     (polygon.bbox().ymax() - polygon.bbox().ymin()) / 2);
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
    q = align(q);
    q = rotate(q, getCenter(q), gen->R(0, 180));
    building.polygon = Polygon_with_holes(q);
    building.center = getCenter(q);

    auto plot = getPlot(q);
    auto a = q.area();
    plot = getPlot(q);
    building.plot = plot;
    return building;
  }

  Polygon getPlot(Polygon q) {
    auto a = q.area();
    auto c = getCenter(q);
    auto plot = centrate(randomPolygon(gen->R(4, 6)), c);

    auto inside = checkInside(q, plot);
    auto n = 0;
    while (!inside) {
      if (plot.area() > a * 7) {
        plot = centrate(randomPolygon(gen->R(4, 7)), c);
      } else {
        plot = centrate(scale(plot, 1.1), c);
      }
      inside = checkInside(q, plot);
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
