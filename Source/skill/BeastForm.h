// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_BEAST_FORM_H_
#define VIGILANTE_BEAST_FORM_H_

#include <string>

#include "Skill.h"
#include "character/Character.h"

namespace vigilante {

class Character;

class BeastForm : public Skill {
 public:
  BeastForm(const std::string& jsonFileName, Character* user);
  virtual ~BeastForm() = default;

  virtual void import(const std::string& jsonFileName) override;  // Skill
  virtual ax::EventKeyboard::KeyCode getHotkey() const override { return _skillProfile.hotkey; }  // Skill
  virtual void setHotkey(ax::EventKeyboard::KeyCode hotkey) override { _skillProfile.hotkey = hotkey; }  // Skill
  virtual bool canActivate() override;  // Skill
  virtual void activate() override;  // Skill
  virtual void deactivate() override;  // Skill

  virtual Skill::Profile& getSkillProfile() override { return _skillProfile; }  // Skill
  virtual const std::string& getName() const override { return _skillProfile.name; }  // Skill
  virtual const std::string& getDesc() const override { return _skillProfile.desc; }  // Skill
  virtual std::string getIconPath() const override;  // Skill

 private:
  Skill::Profile _skillProfile;
  Character* _user{};
  bool _hasActivated{};
  Character::Profile _originalCharacterProfile;
};

}  // namespace vigilante

#endif // VIGILANTE_BEAST_FORM_H_
