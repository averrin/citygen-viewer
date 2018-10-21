#ifndef __BUILDING_H_
#define __BUILDING_H_

#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <list>
#include <memory>

#include <librandom/random.hpp>

typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef Kernel::Point_2 Point_2;
typedef CGAL::Polygon_2<Kernel> Polygon_2;
typedef CGAL::Polygon_with_holes_2<Kernel> Polygon_with_holes_2;
typedef std::list<Polygon_with_holes_2> Pwh_list_2;

class BuildingGenerator {
  std::shared_ptr<R::Generator> gen;

public:
  BuildingGenerator(int seed) {
    gen = std::make_shared<R::Generator>();
    gen->setSeed(seed);
  }
  Polygon_2 randomQuad(int dx = 0, int dy = 0) {
    auto w = gen->R(2, 10);
    auto h = gen->R(2, 10);
    Polygon_2 Q;

    Q.push_back(Point_2(dx, dy));
    Q.push_back(Point_2(dx + w, dy));
    Q.push_back(Point_2(dx + w, dy + h));
    Q.push_back(Point_2(dx, dy + h));

    if (!Q.is_simple()) {
        Q = randomQuad(dx, dy);
    }
    return Q;
  }

  Polygon_with_holes_2 randomBuilding() {
    Polygon_with_holes_2 result;

    auto c = gen->R(1, 5);
    auto q = randomQuad(gen->R(0, 5), gen->R(0, 5));
    for (auto i = 0; i < c - 1; i++) {
      auto p = randomQuad(gen->R(0, 5), gen->R(0, 5));
      if (CGAL::join(p, q, result) && !result.is_unbounded() && result.outer_boundary().is_simple()) {
        q = result.outer_boundary();
      } else {
        i--;
      }
    }
    return result;
  }
};

/*

    Pwh_list_2                  intR;

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
