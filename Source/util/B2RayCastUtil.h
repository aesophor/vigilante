// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_B2_RAY_CAST_UTIL_H_
#define VIGILANTE_B2_RAY_CAST_UTIL_H_

#include <box2d/box2d.h>

namespace vigilante {

class RayCastCallback final : public b2RayCastCallback {
 public:
  virtual float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) {
    _hasHit = true;
    _fixture = fixture;
    _point = point;
    _normal = normal;
    _fraction = fraction;
    return 0;
  }

  inline bool hasHit() const { return _hasHit; }
  inline b2Fixture* getFixture() const { return _fixture; }
  inline const b2Vec2& getPoint() const { return _point; }
  inline const b2Vec2& getNormal() const { return _normal; }
  inline float getFraction() const { return _fraction; }

 private:
  bool _hasHit{};
  b2Fixture* _fixture{};
  b2Vec2 _point;
  b2Vec2 _normal;
  float _fraction;
};

}  // namespace vigilante

#endif  // VIGILANTE_B2_RAY_CAST_UTIL_H_
