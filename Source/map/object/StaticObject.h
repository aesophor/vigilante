// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_STATIC_OBJECT_H_
#define VIGILANTE_STATIC_OBJECT_H_

#include <string>

#include <axmol.h>

#include "StaticActor.h"

namespace vigilante {

class StaticObject : public StaticActor {
 public:
  StaticObject(const std::string& textureResDir,
               const std::string& framesName,
               const float frameInterval,
               const int zOrder)
      : StaticActor{},
        _textureResDir{textureResDir},
        _framesName{framesName},
        _frameInterval{frameInterval},
        _zOrder{zOrder} {}
  virtual ~StaticObject() override = default;

  virtual bool showOnMap(float x, float y) override;  // StaticActor

 private:
  void defineTexture();

  const std::string _textureResDir;
  const std::string _framesName;
  const float _frameInterval;
  const int _zOrder;
};

}  // namespace vigilante

#endif  // VIGILANTE_STATIC_OBJECT_H_
