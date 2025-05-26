// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "MathUtil.h"

#include <array>
#include <cmath>

using namespace std;

namespace requiem::math_util {

optional<float> getSlope(const b2Vec2& point1, const b2Vec2& point2) {
  array<b2Vec2, 2> v = {{point1, point2}};
  sort(v.begin(), v.end(), [](const b2Vec2& p1, const b2Vec2& p2) { return p1.x < p2.x; });

  if (v[0].x == v[1].x) {
    return nullopt;
  }
  return (v[1].y - v[0].y) / (v[1].x - v[0].x);
}

// https://en.wikipedia.org/wiki/Rotation_matrix
b2Vec2 rotateCounterClockwise(const b2Vec2& v, const float degree) {
  const float rad = deg2Rad(degree);
  const float x = std::cos(rad) * v.x - std::sin(rad) * v.y;
  const float y = std::sin(rad) * v.x + std::cos(rad) * v.y;
  return {x, y};
}

}  // namespace requiem::math_util
