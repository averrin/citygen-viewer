#ifndef __MAP_H_
#define __MAP_H_
#include <vector>
#include <citygen/common.hpp>
#include <citygen/area.hpp>
#include <citygen/building.hpp>

namespace CityGen {
class Map {
public:
        std::vector<Building> buildings;
        std::vector<Area> areas;
};
}

#endif // __MAP_H_
