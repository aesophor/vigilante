// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_FX_MANAGER_H_
#define VIGILANTE_FX_MANAGER_H_

#include <filesystem>
#include <string>
#include <unordered_map>

#include <axmol.h>

#include <box2d/box2d.h>

namespace vigilante {

class Character;

class FxManager final {
 public:
  void createDustFx(const Character* c);
  void createHitFx(const Character* c);
  ax::Sprite* createHintBubbleFx(const b2Body* body,
                                 const std::string& framesName);

  void removeFx(ax::Sprite* sprite);

  ax::Sprite* createAnimation(const std::filesystem::path& textureResDirPath,
                              const std::string& framesName,
                              const float x,
                              const float y,
                              const unsigned int loopCount = 1,
                              const float frameInterval = 10.0f);

 private:
  std::unordered_map<std::filesystem::path, ax::Animation*> _animationCache;
};

}  // namespace vigilante

#endif  // VIGILANTE_FX_MANAGER_H_
