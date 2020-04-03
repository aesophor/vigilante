// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_FX_MANAGER_H_
#define VIGILANTE_FX_MANAGER_H_

#include <unordered_map>
#include <string>

#include <cocos2d.h>

namespace vigilante {

class FxManager {
 public:
  explicit FxManager(cocos2d::Layer* gameMapLayer);
  virtual ~FxManager() = default;

  void createFx(const std::string& textureResDir, const std::string& framesName,
                float x, float y);

 private:
  static std::string getSpritesheetFileName(const std::string& textureResDir);

  cocos2d::Layer* _gameMapLayer;
  std::unordered_map<std::string, cocos2d::Animation*> _animationCache;
};

} // namespace vigilante

#endif // VIGILANTE_FX_MANAGER_H_
