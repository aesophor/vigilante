// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_STATIC_ACTOR_H_
#define VIGILANTE_STATIC_ACTOR_H_

#include <vector>

#include "cocos2d.h"

namespace vigilante {

// A static actor is an abstract class which represents a game entity
// consisting of the following members:
// 1. a sprite placed at a specific position
// 2. a spritesheet and several animations

class StaticActor {
 public:
  virtual ~StaticActor() = default;

  // Show and hide the sprite in the game map.
  virtual void showOnMap(float x, float y);
  virtual void removeFromMap();
  virtual void setPosition(float x, float y);

  cocos2d::Sprite* getBodySprite() const;
  cocos2d::SpriteBatchNode* getBodySpritesheet() const;

  // Create animation from Texture/{category}/{entityName}/{entityName}_{frameName}
  // e.g., to create the animation of "slime" "killed", pass the following arguments
  // to this function: ("Texture/character/slime", "killed", 3.0f / kPpm).
  //
  // If the target animation cannot be created, the fallback animation will be used
  // instead. If the user did not provide a fallback animation, a std::runtime_error
  // will be thrown.
  //
  // IMPORTANT: animations created with this utility method should be release()d!
  //
  // @param textureResDir: the path to texture resource directory
  // @param framesName: the name of the frames
  // @param interval: the interval between each frame
  // @param fallback: the fallback animation (if the target animation cannot be created)
  // @return: an pointer to the instance of cocos2d::Animation
  static cocos2d::Animation* createAnimation(const std::string& textureResDir, std::string framesName,
                                             float interval, cocos2d::Animation* fallback=nullptr);

  // The texture resources under Resources/Texture/ has the following rules:
  //
  // Texture/character/player/player_attacking/0.png
  // |______________________| |____| |_______| |___|
  //      textureResDir          |   framesName
  //                      framesNamePrefix
  //
  // As you can see, each framesName (e.g., attacking) is preceded by a prefix,
  // this is to **prevent frames name collision** in cocos2d::SpriteFrameCache!
  //
  // The following utility method takes in textureResPath as a parameter
  // and returns the last dirname.
  // e.g., "Texture/character/player" -> "player"
  static std::string getLastDirName(const std::string& directory);

 protected:
  StaticActor(size_t numAnimations=1);

  bool _isShownOnMap;
  cocos2d::Sprite* _bodySprite;
  cocos2d::SpriteBatchNode* _bodySpritesheet;
  std::vector<cocos2d::Animation*> _bodyAnimations;
};

} // namespace vigilante

#endif // VIGILANTE_STATIC_ACTOR_H_
