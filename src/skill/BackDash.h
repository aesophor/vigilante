// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_BACK_DASH_H_
#define VIGILANTE_BACK_DASH_H_

#include <string>

#include "Skill.h"

namespace vigilante {

class Character;

class BackDash : public Skill {
 public:
  BackDash(const std::string& jsonFileName, Character* user);
  virtual ~BackDash() = default;

  virtual void import(const std::string& jsonFileName) override; // Skill
  virtual cocos2d::EventKeyboard::KeyCode getHotkey() const override; // Skill
  virtual void setHotkey(cocos2d::EventKeyboard::KeyCode hotkey) override; // Skill
  virtual bool canActivate() override; // Skill
  virtual void activate() override; // Skill

  virtual Skill::Profile& getSkillProfile() override; // Skill
  virtual const std::string& getName() const override; // Skill
  virtual const std::string& getDesc() const override; // Skill
  virtual std::string getIconPath() const override; // Skill

 private:
  Skill::Profile _skillProfile;
  Character* _user;

  bool _hasActivated;
};

} // namespace vigilante

#endif // VIGILANTE_BACK_DASH_H_
