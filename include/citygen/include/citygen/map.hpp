#ifndef __MAP_H_
#define __MAP_H_
#include <vector>
#include <citygen/common.hpp>
#include <citygen/building.hpp>

namespace CityGen {
class Map {
public:
        std::vector<Building> buildings;
};
}

#endif // __MAP_H_
