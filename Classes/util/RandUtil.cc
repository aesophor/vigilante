#include "RandUtil.h"

#include <ctime>
#include <random>


namespace vigilante {

namespace rand_util {

void init() {
  srand(time(nullptr));
}

int randInt(int min, int max) {
  return min + (rand() % static_cast<int>(max - min + 1));
}

} // namespace rand_util

} // namespace vigilante
