#ifndef __GENERATOR_H_
#define __GENERATOR_H_
#include <memory>

#include <librandom/random.hpp>

#include <citygen/building.hpp>
#include <citygen/map.hpp>

namespace CityGen {

class Generator {
  std::shared_ptr<CityGen::BuildingGenerator> buildingGenerator;
  std::shared_ptr<R::Generator> gen;

public:
  Generator(int seed) {
    gen = std::make_shared<R::Generator>();
    gen->setSeed(seed);
    buildingGenerator = std::make_shared<CityGen::BuildingGenerator>(gen->seed);
  }
  std::shared_ptr<CityGen::Map> createMap();
};

} // namespace CityGen

#endif // __GENERATOR_H_
