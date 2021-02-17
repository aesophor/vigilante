// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_RAND_UTIL_H_
#define VIGILANTE_RAND_UTIL_H_

namespace vigilante {

namespace rand_util {

void init();
int randInt(int min=0, int max=1);
float randFloat(float min=0.0f, float max=1.0f);

} // namespace rand_util

} // namespace vigilante

#endif // VIGILANTE_RAND_UTIL_H_
