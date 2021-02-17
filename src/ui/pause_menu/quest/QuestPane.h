// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_QUEST_PANE_H_
#define VIGILANTE_QUEST_PANE_H_

#include <memory>

#include <cocos2d.h>
#include <ui/UIImageView.h>
#include "ui/TabView.h"
#include "ui/pause_menu/AbstractPane.h"
#include "ui/pause_menu/quest/QuestListView.h"

namespace vigilante {

class QuestPane : public AbstractPane {
 public:
  explicit QuestPane(PauseMenu* pauseMenu);
  virtual ~QuestPane() = default;

  virtual void update() override;
  virtual void handleInput() override;

 private:
  cocos2d::ui::ImageView* _background;
  std::unique_ptr<TabView> _tabView;
  std::unique_ptr<QuestListView> _questListView;
};

} // namespace vigilante

#endif // VIGILANTE_QUEST_PANE_H_
