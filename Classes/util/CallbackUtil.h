// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_CALLBACK_UTIL_H_
#define VIGILANTE_CALLBACK_UTIL_H_

#include <functional>

#include <cocos2d.h>

namespace vigilante {

namespace callback_util {

void init(cocos2d::Scene* scene);
void runAfter(const std::function<void()>& func, float delay);

}  // namespace callback_util

}  // namespace vigilante

#endif  // VIGILANTE_CALLBACK_UTIL_H_
