#ifndef __SHAPES_H_
#define __SHAPES_H_
#include <citygen/common.hpp>

namespace CityGen {
class Shapes : public WithRandomGenerator {
public:
  Shapes(std::shared_ptr<R::Generator> g) : WithRandomGenerator(g) {
    }
  static bool checkInside(Point pt, Polygon p) {
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

  static bool checkInside(Polygon q, Polygon p) {
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

  static Polygon centrate(Polygon p, Point origin) {
    auto c = getCenter(p);
    auto dx = origin.x() - c.x();
    auto dy = origin.y() - c.y();
    Polygon t;
    for (auto point : p.container()) {
      t.push_back(Point(point.x() + dx, point.y() + dy));
    }
    return t;
  }

  static Polygon scale(Polygon p, float mod) {
    Polygon t;
    for (auto point : p.container()) {
      t.push_back(Point(point.x() * mod, point.y() * mod));
    }
    return t;
  }

  static Polygon translate(Polygon p, Point origin) {
    Polygon t;
    for (auto point : p.container()) {
      t.push_back(Point(point.x() + origin.x(), point.y() + origin.y()));
    }
    return t;
  }

  Polygon randomPolygon(int c, float origin = 5) {
      updateSeed();
    Polygon p;

    CGAL::random_convex_set_2(c, std::back_inserter(p),
                              Point_generator(origin, cgalRandom));
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

  // static Polygon distort(Polygon p, float min, float max) {
  //   Polygon d;

  //   for (auto point : p.container()) {
  //     d.push_back(Point(abs(point.x() + gen->R(min, max)),
  //                       abs(point.y() + gen->R(min, max))));
  //   }

  //   return d;
  // }

  static Polygon align(Polygon p) {
    Polygon d;
    auto dx = p.bbox().xmin();
    auto dy = p.bbox().ymin();

    for (auto point : p.container()) {
      d.push_back(Point(point.x() - dx, point.y() - dy));
    }

    return d;
  }

  static Point rotate(Point p, Point origin, float angle) {
    auto tx = p.x() - origin.x();
    auto ty = p.y() - origin.y();
    auto rx = tx * cos(angle) - ty * sin(angle);
    auto ry = tx * sin(angle) + ty * cos(angle);
    return Point(rx + origin.x(), ry + origin.y());
  }

  static Polygon rotate(Polygon p, Point origin, float angle) {
    Polygon d;
    for (auto point : p.container()) {
      d.push_back(rotate(point, origin, angle));
    }
    return d;
  }

  static Point getCenter(Polygon polygon) {
    return Point(polygon.bbox().xmin() +
                     (polygon.bbox().xmax() - polygon.bbox().xmin()) / 2,
                 polygon.bbox().ymin() +
                     (polygon.bbox().ymax() - polygon.bbox().ymin()) / 2);
  }
};
} // namespace CityGen

#endif // __SHAPES_H_
