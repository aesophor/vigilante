// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_MAGICAL_MISSILE_H_
#define VIGILANTE_MAGICAL_MISSILE_H_

#include <string>

#include <axmol.h>
#include <box2d/box2d.h>

#include "DynamicActor.h"
#include "Skill.h"
#include "Projectile.h"

namespace vigilante {

class Character;

class MagicalMissile : public DynamicActor, public Skill, public Projectile {
 public:
  enum AnimationType {
    LAUNCH_FX,
    FLYING,
    ON_HIT,
    SIZE
  };

  MagicalMissile(const std::string& jsonFileName, Character* user);
  virtual ~MagicalMissile() = default;

  virtual bool showOnMap(float x, float y) override;  // DynamicActor
  virtual void update(float delta) override;  // DynamicActor

  virtual Character* getUser() const override { return _user; }  // Projectile
  virtual int getDamage() const override;  // Projectile
  virtual void onHit(Character* target) override;  // Projectile

  virtual void import(const std::string& jsonFileName) override;  // Skill
  virtual ax::EventKeyboard::KeyCode getHotkey() const override { return _skillProfile.hotkey; }  // Skill
  virtual void setHotkey(ax::EventKeyboard::KeyCode hotkey) override { _skillProfile.hotkey = hotkey; }  // Skill
  virtual bool canActivate() override;  // Skill
  virtual void activate() override;  // Skill

  virtual Skill::Profile& getSkillProfile() override { return _skillProfile; }  // Skill
  virtual const std::string& getName() const override { return _skillProfile.name; }  // Skill
  virtual const std::string& getDesc() const override { return _skillProfile.desc; }  // Skill
  virtual std::string getIconPath() const override;  // Skill
  
 private:
  virtual void defineBody(b2BodyType bodyType,
                          float x,
                          float y,
                          short categoryBits,
                          short maskBits);

  virtual void defineTexture(const std::string& textureResPath, float x, float y);

  Skill::Profile _skillProfile;
  Character* _user{};
  float _flyingSpeed{};
  bool _hasActivated{};
  bool _hasHit{};
  ax::Sprite* _launchFxSprite{};
};

}  // namespace vigilante

#endif  // VIGILANTE_MAGICAL_MISSILE_H_
