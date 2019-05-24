#ifndef VIGILANTE_ICE_SPIKE_H_
#define VIGILANTE_ICE_SPIKE_H_

#include <string>
#include <functional>

#include "cocos2d.h"
#include "Box2D/Box2D.h"

#include "DynamicActor.h"
#include "Spell.h"

namespace vigilante {

class Character;

class IceSpike : public DynamicActor, public Spell {
 public:
  IceSpike(Character* spellUser, float flyingSpeed=2.0f);
  virtual ~IceSpike();

  virtual void update(float delta) override;
  virtual void setPosition(float x, float y) override;
  virtual void showOnMap(float x, float y) override;
  virtual void removeFromMap() override;

  virtual void activate() override; // activate the spell
  virtual void onHit() override;

  virtual Character* getSpellUser() const override;
  virtual int getDamage() const override;

 private:
  virtual void defineBody(b2BodyType bodyType, short categoryBits, short maskBits, float x, float y);
  virtual void defineTexture(const std::string& textureResPath, float x, float y);

  Character* _spellUser;
  int _damage;
  float _flyingSpeed;
  bool _hasActivated;

  cocos2d::Sprite* _launchFxSprite; // sprite of launching fx

  enum AnimationType {
    LAUNCH_FX,
    FLYING,
    ON_HIT,
    SIZE
  };
};

} // namespace vigilante

#endif // VIGILANTE_ICE_SPIKE_H_
