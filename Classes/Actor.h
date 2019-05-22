#ifndef VIGILANTE_ACTOR_H_
#define VIGILANTE_ACTOR_H_

#include <vector>
#include <string>

#include "cocos2d.h"
#include "Box2D/Box2D.h"

namespace vigilante {

// An actor is an abstract class which represents a game entity
// consisting of the following members:
// 1. a b2Body (with one or more b2Fixtures attached to it)
// 2. a sprite synchronized with its b2Body
// 3. a spritesheet and several body animations
//
// If you need more sprites and animations (e.g., equipment displayed on top of a character),
// then manually subclass "Actor" and declare them as the members of your subclass.
//
// Notes on the pure virtual methods:
// 1. Actor::update(float delta) should "sync the sprites with its b2Body".
// 2. <insert next item...>

class Actor {
 public:
  Actor(size_t fixturesSize=1, size_t animationsSize=1);
  virtual ~Actor() = default;

  virtual void update(float delta) = 0;
  virtual void setPosition(float x, float y) = 0;

  b2Body* getBody() const;
  cocos2d::Sprite* getBodySprite() const;
  cocos2d::SpriteBatchNode* getBodySpritesheet() const;

 protected:
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
  static std::string extractTrailingDir(const std::string& directory);

  b2Body* _body; // users should manually destory _body in subclass!
  std::vector<b2Fixture*> _fixtures;

  cocos2d::Sprite* _bodySprite;
  cocos2d::SpriteBatchNode* _bodySpritesheet;
  std::vector<cocos2d::Animation*> _bodyAnimations;
};

} // namespace vigilante

#endif // VIGILANTE_ACTOR_H_
