// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_FX_MANAGER_H_
#define VIGILANTE_FX_MANAGER_H_

#include <string>
#include <unordered_map>

#include <axmol.h>
#include <box2d/box2d.h>

namespace vigilante {

// Forward Declaration
class Character;

class FxManager final {
 public:
  void createDustFx(Character* c);
  ax::Sprite* createHintBubbleFx(const b2Body* body,
                                 const std::string& framesName);

  void removeFx(ax::Sprite* sprite);

 private:
  ax::Sprite* createFx(const std::string& textureResDir,
                       const std::string& framesName,
                       float x,
                       float y,
                       unsigned int loopCount=1,
                       float frameInterval=10.0f);

  static std::string getSpritesheetFileName(const std::string& textureResDir);

  std::unordered_map<std::string, ax::Animation*> _animationCache;
};

}  // namespace vigilante

#endif  // VIGILANTE_FX_MANAGER_H_
