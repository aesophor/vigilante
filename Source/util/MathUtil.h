// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_MATH_UTIL_H_
#define VIGILANTE_MATH_UTIL_H_

#include <numbers>
#include <optional>

#include <box2d/box2d.h>

namespace vigilante::math_util {

inline float rad2Deg(const float radian) { return radian * 180.0f / std::numbers::pi; }
inline float deg2Rad(const float degree) { return degree * std::numbers::pi / 180.0f; }

std::optional<float> getSlope(const b2Vec2& p1, const b2Vec2& p2);
b2Vec2 rotateCounterClockwise(const b2Vec2& v, const float degree);

}  // namespace vigilante::math_util

#endif  // VIGILANTE_MATH_UTIL_H_
