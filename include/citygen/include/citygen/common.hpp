#ifndef __COMMON_H_
#define __COMMON_H_

#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/random_convex_set_2.h>

typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef Kernel::Point_2 Point;
typedef CGAL::Polygon_2<Kernel> Polygon;
typedef CGAL::Polygon_with_holes_2<Kernel> Polygon_with_holes;

typedef CGAL::Random_points_in_square_2<
     Point,
     CGAL::Creator_uniform_2< double, Point > >
                                    Point_generator;

#endif // __COMMON_H_
