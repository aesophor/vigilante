// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "SkillListView.h"

#include <ui/UIImageView.h>
#include "input/InputManager.h"
#include "ui/pause_menu/PauseMenu.h"
#include "ui/pause_menu/PauseMenuDialog.h"

using std::deque;
using std::vector;
using cocos2d::EventKeyboard;

namespace vigilante {

SkillListView::SkillListView(PauseMenu* pauseMenu, int visibleItemCount)
    : ListView<Skill*>(pauseMenu, visibleItemCount) {}


void SkillListView::confirm() {
  Skill* skill = getSelectedObject();
  if (!skill) {
    return;
  }

  PauseMenuDialog* dialog = _pauseMenu->getDialog();
  dialog->reset();
  dialog->setMessage("What would you like to do with " + skill->getName() + "?");

  dialog->setOption(0, true, "Assign", [=]() {
    dialog->reset();
    dialog->setMessage("Press a key to assign to...");
    dialog->setOption(2, true, "Cancel");
    dialog->show();
    InputManager::getInstance()->promptHotkey(skill, dialog);
  });

  if (static_cast<bool>(skill->getHotkey())) {
    dialog->setOption(1, true, "Clear", [=]() {
      InputManager::getInstance()->clearHotkeyAction(skill->getHotkey());
      showSkills();
    });
  }

  dialog->setOption(2, true, "Cancel");
  dialog->show();
}


void SkillListView::showSkills() {
  // Copy all skills into local deque.
  const vector<Skill*>& skills = _pauseMenu->getCharacter()->getSkills();
  _objects = deque<Skill*>(skills.begin(), skills.end());

  _firstVisibleIndex = 0;
  _current = 0;
  showFrom(_firstVisibleIndex);

  // If the inventory size isn't empty, select the first item by default.
  if (_objects.size() > 0) {
    _listViewItems[0]->setSelected(true);
    _descLabel->setString(_objects[_current]->getDesc());
  } else {
    _descLabel->setString("");
  }
}

} // namespace vigilante
