// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_UI_PAUSE_MENU_SKILL_SKILL_LIST_VIEW_H_
#define VIGILANTE_UI_PAUSE_MENU_SKILL_SKILL_LIST_VIEW_H_

#include <string>

#include "skill/Skill.h"
#include "ui/ListView.h"

namespace vigilante {

class PauseMenu;

class SkillListView final : public ListView<Skill*> {
 public:
  explicit SkillListView(PauseMenu* pauseMenu);
  virtual ~SkillListView() = default;

  virtual void confirm() override;  // ListView<Skill*>
  virtual void selectUp() override;  // ListView<Skill*>
  virtual void selectDown() override;  // ListView<Skill*>

  void showSkillsByType(Skill::Type skillType);

 private:
  PauseMenu* _pauseMenu;
  ax::Label* _descLabel;
};

}  // namespace vigilante

#endif  // VIGILANTE_UI_PAUSE_MENU_SKILL_SKILL_LIST_VIEW_H_
