// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_FX_MANAGER_H_
#define VIGILANTE_FX_MANAGER_H_

#include <string>
#include <unordered_map>

#include <cocos2d.h>
#include <Box2D/Box2D.h>

namespace vigilante {

// Forward Declaration
class Character;

class FxManager {
 public:
  static FxManager* getInstance();
  virtual ~FxManager() = default;

  void createDustFx(Character* c);
  cocos2d::Sprite* createHintBubbleFx(const b2Body* body,
                                      const std::string& framesName);

  void removeFx(cocos2d::Sprite* sprite);

 private:
  FxManager() = default;

  cocos2d::Sprite* createFx(const std::string& textureResDir,
                            const std::string& framesName,
                            float x,
                            float y,
                            unsigned int loopCount=1,
                            float frameInterval=10.0f);

  static std::string getSpritesheetFileName(const std::string& textureResDir);

  std::unordered_map<std::string, cocos2d::Animation*> _animationCache;
};

}  // namespace vigilante

#endif  // VIGILANTE_FX_MANAGER_H_
