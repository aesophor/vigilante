// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_MAP_OBJECT_STATIC_OBJECT_H_
#define REQUIEM_MAP_OBJECT_STATIC_OBJECT_H_

#include <string>

#include <axmol.h>

#include "StaticActor.h"

namespace requiem {

class StaticObject : public StaticActor {
 public:
  StaticObject(const std::string& textureResDir,
               const std::string& framesName,
               const float frameInterval,
               const bool flipped,
               const int zOrder)
      : StaticActor{},
        _textureResDir{textureResDir},
        _framesName{framesName},
        _frameInterval{frameInterval},
        _flipped{flipped},
        _zOrder{zOrder} {}
  virtual ~StaticObject() override = default;

  virtual bool showOnMap(float x, float y) override;  // StaticActor

 private:
  void defineTexture();

  const std::string _textureResDir;
  const std::string _framesName;
  const float _frameInterval;
  const bool _flipped;
  const int _zOrder;
};

}  // namespace requiem

#endif  // REQUIEM_MAP_OBJECT_STATIC_OBJECT_H_
