// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_SKILL_LIST_VIEW_H_
#define VIGILANTE_SKILL_LIST_VIEW_H_

#include <string>

#include "skill/Skill.h"
#include "ui/ListView.h"

namespace vigilante {

class PauseMenu;

class SkillListView : public ListView<Skill*> {
 public:
  explicit SkillListView(PauseMenu* pauseMenu);
  virtual ~SkillListView() = default;

  virtual void confirm() override; // ListView<Skill*>
  virtual void selectUp() override; // ListView<Skill*>
  virtual void selectDown() override; // ListView<Skill*>

  void showSkills();

 private:
  PauseMenu* _pauseMenu;
  cocos2d::Label* _descLabel;
};

} // namespace vigilante

#endif // VIGILANTE_SKILL_LIST_VIEW_H_
