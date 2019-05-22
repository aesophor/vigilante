#ifndef VIGILANTE_ICE_SPIKE_H_
#define VIGILANTE_ICE_SPIKE_H_

#include <string>
#include <functional>

#include "cocos2d.h"
#include "Box2D/Box2D.h"

namespace vigilante {

class Character;

class IceSpike {
 public:
  IceSpike(Character* spellUser);
  virtual ~IceSpike();
  virtual void update(float delta);

  virtual void reposition(float x, float y);
  virtual void activate(float speed=2.0); // activate the spell
  virtual void onHit();

  Character* getSpellUser() const;
  const int getDamage() const;

  b2Body* getBody() const;
  b2Fixture* getFixture() const;

  cocos2d::Sprite* getSprite() const;
  cocos2d::SpriteBatchNode* getSpritesheet() const;

 private:
  virtual void defineBody(b2BodyType bodyType, short categoryBits, short maskBits, float x, float y);
  virtual void defineTexture(const std::string& textureResPath, float x, float y);
  cocos2d::Animation* createAnimation(const std::string& textureResPath, std::string frameName, float delay) const;

  Character* _spellUser;
  const int _damage;
  bool _hasActivated;

  b2Body* _body;
  b2Fixture* _fixture;

  cocos2d::SpriteBatchNode* _spritesheet;

  cocos2d::Sprite* _launchFxSprite; // sprite of launching fx
  cocos2d::Sprite* _sprite; // sprite of the actual projectile

  cocos2d::Animation* _launchFxAnimation;
  cocos2d::Animation* _flyingAnimation;
  cocos2d::Animation* _onHitAnimation;
};

} // namespace vigilante

#endif // VIGILANTE_ICE_SPIKE_H_
