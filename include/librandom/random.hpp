#ifndef __RANDOM_H_
#define __RANDOM_H_

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <random>

namespace R {
// float R(float min, float max) {
//   return min + static_cast<float>(rand()) /
//                    (static_cast<float>(RAND_MAX / (max - min)));
//   ;
// }
// float R() { return static_cast<float>(rand()) / static_cast<float>(RAND_MAX); }

// int Z(int min, int max) { return rand() % (max - min + 1) + min; }

// int Z() { return rand() % 100; }

// int N(int mean, int dev) {
//   std::normal_distribution<> d{double(mean), double(dev)};

//   return std::round(d(gen));
// }

class Generator {
private:
  std::mt19937 *gen;

public:
  int seed;
  Generator() { updateSeed(); }

  void updateSeed() {

    setSeed(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch())
                .count());
  }

  void setSeed(int s) {
    seed = s;
    gen = new std::mt19937(seed);
  }

  int R() {
    std::uniform_int_distribution<> dis(0, RAND_MAX);
    return dis(*gen);
  }

  int R(float min, float max) {
    std::uniform_real_distribution<> dis(min, max);
    return dis(*gen);
  }

  int R(int min, int max) {
    std::uniform_int_distribution<> dis(min, max);
    return dis(*gen);
  }
};
} // namespace R

#endif // __RANDOM_H_
