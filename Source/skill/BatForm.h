// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_SKILL_BAT_FORM_H_
#define REQUIEM_SKILL_BAT_FORM_H_

#include <string>

#include "Skill.h"

namespace requiem {

class Character;

class BatForm final : public Skill {
 public:
  BatForm(const std::filesystem::path& jsonFilePath, Character* user);
  virtual ~BatForm() = default;

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
  Skill::Profile _skillProfile;
  Character* _user{};
  bool _hasActivated{};
};

}  // namespace requiem

#endif // REQUIEM_SKILL_BAT_FORM_H_
