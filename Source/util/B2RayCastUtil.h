// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_UTIL_B2_RAY_CAST_UTIL_H_
#define VIGILANTE_UTIL_B2_RAY_CAST_UTIL_H_

#include <functional>

#include <box2d/box2d.h>

namespace vigilante {

class B2RayCastCallback final : public b2RayCastCallback {
 public:
  using Callback = std::function<float(b2Fixture*, const b2Vec2&, const b2Vec2&, float)>;

  B2RayCastCallback(Callback&& callback) : _callback{std::move(callback)} {}

  virtual float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) {
    const float result = _callback(fixture, point, normal, fraction);
    if (!result) {
      _hasHit = true;
    }
    return result;
  }

  inline bool hasHit() const { return _hasHit; }

 private:
  Callback _callback;
  bool _hasHit{};
};

}  // namespace vigilante

#endif  // VIGILANTE_UTIL_B2_RAY_CAST_UTIL_H_
