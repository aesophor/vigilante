// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_UTIL_RAND_UTIL_H_
#define VIGILANTE_UTIL_RAND_UTIL_H_

namespace vigilante::rand_util {

void init();
int randInt(int min=0, int max=1);
float randFloat(float min=0.0f, float max=1.0f);

}  // namespace vigilante::rand_util

#endif  // VIGILANTE_UTIL_RAND_UTIL_H_
