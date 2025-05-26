// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_UTIL_RAND_UTIL_H_
#define REQUIEM_UTIL_RAND_UTIL_H_

namespace requiem::rand_util {

void init();
int randInt(int min=0, int max=1);
float randFloat(float min=0.0f, float max=1.0f);

}  // namespace requiem::rand_util

#endif  // REQUIEM_UTIL_RAND_UTIL_H_
