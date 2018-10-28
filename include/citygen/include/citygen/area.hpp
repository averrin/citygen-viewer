#ifndef __AREA_H_
#define __AREA_H_
#include <citygen/common.hpp>

namespace CityGen {
enum class AreaType {
  CITY,
  WALLED,
};

class Area : public Primitive {
public:
  AreaType type;
};
} // namespace CityGen
#endif // __AREA_H_
