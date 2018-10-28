#ifndef __GENERATOR_H_
#define __GENERATOR_H_
#include <memory>

#include <librandom/random.hpp>

#include <citygen/common.hpp>
#include <citygen/building.hpp>
#include <citygen/map.hpp>

namespace CityGen {

class Generator : public WithRandomGenerator {
  std::shared_ptr<CityGen::BuildingGenerator> buildingGenerator;

public:
  Generator(std::shared_ptr<R::Generator> g) : WithRandomGenerator(g) {
    buildingGenerator = std::make_shared<CityGen::BuildingGenerator>(gen);
  }
  std::shared_ptr<CityGen::Map> createMap(float width, float height);
};

} // namespace CityGen

#endif // __GENERATOR_H_
