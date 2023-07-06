// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_BAT_FORM_H_
#define VIGILANTE_BAT_FORM_H_

#include <string>

#include "Skill.h"

namespace vigilante {

class Character;

class BatForm : public Skill {
 public:
  BatForm(const std::string& jsonFileName, Character* user);
  virtual ~BatForm() = default;

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
  Skill::Profile _skillProfile;
  Character* _user;

  bool _hasActivated;
};

}  // namespace vigilante

#endif // VIGILANTE_BAT_FORM_H_
