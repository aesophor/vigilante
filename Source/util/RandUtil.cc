// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "RandUtil.h"

#include <ctime>
#include <random>

namespace vigilante::rand_util {

void init() {
  srand(time(nullptr));
}

int randInt(int min, int max) {
  return (rand() % (max + 1 - min)) + min;
}

float randFloat(float min, float max) {
  return (float(rand()) / (float(RAND_MAX) + 1.0)) * (max - min) + min;
}

}  // namespace vigilante::rand_util
