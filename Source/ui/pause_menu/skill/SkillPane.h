// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_UI_PAUSE_MENU_SKILL_SKILL_PANE_H_
#define REQUIEM_UI_PAUSE_MENU_SKILL_SKILL_PANE_H_

#include <memory>

#include <axmol.h>
#include <ui/UIImageView.h>

#include "ui/TabView.h"
#include "ui/pause_menu/AbstractPane.h"
#include "ui/pause_menu/skill/SkillListView.h"

namespace requiem {

class SkillPane final : public AbstractPane {
 public:
  explicit SkillPane(PauseMenu* pauseMenu);
  virtual ~SkillPane() = default;

  virtual void update() override;
  virtual void handleInput() override;

 private:
  ax::ui::ImageView* _background;
  std::unique_ptr<TabView> _tabView;
  std::unique_ptr<SkillListView> _skillListView;
};

}  // namespace requiem

#endif  // REQUIEM_UI_PAUSE_MENU_SKILL_SKILL_PANE_H_
