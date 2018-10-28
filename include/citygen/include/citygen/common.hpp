#ifndef __COMMON_H_
#define __COMMON_H_

#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/random_convex_set_2.h>

#include <cmath>
#include <librandom/random.hpp>
#include <memory>

typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef Kernel::Point_2 Point;
typedef CGAL::Polygon_2<Kernel> Polygon;
typedef CGAL::Polygon_with_holes_2<Kernel> Polygon_with_holes;

typedef CGAL::Random_points_in_square_2<Point,
                                        CGAL::Creator_uniform_2<double, Point>>
    Point_generator;

namespace CityGen {
class Primitive {
public:
  Polygon_with_holes polygon;
  float x;
  float y;
};

class WithRandomGenerator {
protected:
  CGAL::Random cgalRandom;
  std::shared_ptr<R::Generator> gen;
  int lastSeed;

  void updateSeed() {
    if (gen->seed != lastSeed) {
      cgalRandom = CGAL::Random(gen->seed);
      lastSeed = gen->seed;
    }
  }

public:
  WithRandomGenerator(std::shared_ptr<R::Generator> g) : gen(g) {
    cgalRandom = CGAL::Random(gen->seed);
  }
};
} // namespace CityGen

#endif // __COMMON_H_
