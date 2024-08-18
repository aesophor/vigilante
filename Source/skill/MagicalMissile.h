// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_SKILL_MAGICAL_MISSILE_H_
#define VIGILANTE_SKILL_MAGICAL_MISSILE_H_

#include <string>

#include <axmol.h>
#include <box2d/box2d.h>

#include "DynamicActor.h"
#include "Skill.h"
#include "Projectile.h"

namespace vigilante {

class Character;

class MagicalMissile final : public DynamicActor, public Skill, public Projectile {
 public:
  enum AnimationType {
    LAUNCH_FX,
    FLYING,
    ON_HIT,
    SIZE
  };

  MagicalMissile(const std::filesystem::path& jsonFilePath, Character* user, const bool onGround);
  virtual ~MagicalMissile() = default;

  virtual bool showOnMap(float x, float y) override;  // DynamicActor
  virtual void update(const float delta) override;  // DynamicActor

  virtual Character* getUser() const override { return _user; }  // Projectile
  virtual int getDamage() const override;  // Projectile
  virtual void onHit(Character* target) override;  // Projectile

  virtual void import(const std::filesystem::path& jsonFilePath) override;  // Skill
  virtual ax::EventKeyboard::KeyCode getHotkey() const override { return _skillProfile.hotkey; }  // Skill
  virtual void setHotkey(ax::EventKeyboard::KeyCode hotkey) override { _skillProfile.hotkey = hotkey; }  // Skill
  virtual bool canActivate() override;  // Skill
  virtual void activate() override;  // Skill
  virtual void deactivate() override {}  // Skill

  virtual Skill::Profile& getSkillProfile() override { return _skillProfile; }  // Skill
  virtual const std::string& getName() const override { return _skillProfile.name; }  // Skill
  virtual const std::string& getDesc() const override { return _skillProfile.desc; }  // Skill
  virtual std::filesystem::path getIconPath() const override;  // Skill

 private:
  virtual void defineBody(b2BodyType bodyType,
                          float x,
                          float y,
                          short categoryBits,
                          short maskBits);

  virtual void defineTexture(const std::filesystem::path& textureResDirPath, float x, float y);

  Skill::Profile _skillProfile;
  Character* _user{};
  bool _isOnGround{};
  float _flyingSpeed{};
  bool _hasActivated{};
  bool _hasHit{};
  ax::Sprite* _launchFxSprite{};
};

}  // namespace vigilante

#endif  // VIGILANTE_SKILL_MAGICAL_MISSILE_H_
