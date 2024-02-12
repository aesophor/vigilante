// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_TELEPORT_STRIKE_H_
#define VIGILANTE_TELEPORT_STRIKE_H_

#include <optional>
#include <string>

#include <box2d/box2d.h>

#include "Skill.h"

namespace vigilante {

class Character;

class TeleportStrike : public Skill {
 public:
  TeleportStrike(const std::string& jsonFilePath, Character* user);

  virtual void import(const std::string& jsonFilePath) override; // Skill
  virtual ax::EventKeyboard::KeyCode getHotkey() const override { return _skillProfile.hotkey; } // Skill
  virtual void setHotkey(ax::EventKeyboard::KeyCode hotkey) override { _skillProfile.hotkey = hotkey; } // Skill
  virtual bool canActivate() override; // Skill
  virtual void activate() override; // Skill
  virtual void deactivate() override {}  // Skill

  virtual Skill::Profile& getSkillProfile() override { return _skillProfile; } // Skill
  virtual const std::string& getName() const override { return _skillProfile.name; } // Skill
  virtual const std::string& getDesc() const override { return _skillProfile.desc; } // Skill
  virtual std::string getIconPath() const override; // Skill

 private:
  Character* getClosestEnemyWithinDist(const float maxEuclideanDist) const;
  std::optional<b2Vec2> determineTeleportDest(Character* target) const;

  Skill::Profile _skillProfile;
  Character* _user{};
  bool _hasActivated{};
};

} // namespace vigilante

#endif // VIGILANTE_TELEPORT_STRIKE_H_
