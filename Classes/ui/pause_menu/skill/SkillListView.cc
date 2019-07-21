// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "SkillListView.h"

#include "input/InputManager.h"
#include "input/Keybindable.h"
#include "ui/pause_menu/PauseMenu.h"
#include "ui/pause_menu/PauseMenuDialog.h"

#define VISIBLE_ITEM_COUNT 5
#define WIDTH vigilante::kVirtualWidth / 2
#define REGULAR_BG vigilante::asset_manager::kItemRegular
#define HIGHLIGHTED_BG vigilante::asset_manager::kItemHighlighted

using std::deque;
using std::vector;
using std::string;
using cocos2d::Label;
using cocos2d::EventKeyboard;

namespace vigilante {

SkillListView::SkillListView(PauseMenu* pauseMenu)
    : ListView<Skill*>(pauseMenu, VISIBLE_ITEM_COUNT, WIDTH, REGULAR_BG, HIGHLIGHTED_BG) {

  // _onSelect is called at the end of ListView<T>::ListViewItem::setObject()
  // see ui/ListView.h
  this->_onSelect = [](ListViewItem* listViewItem, Skill* skill) {
    Label* label = listViewItem->getLabel();

    // Display skill hotkey (if defined).
    // Skills are Keybindable, see skill/Skill.h
    Keybindable* keybindable = dynamic_cast<Keybindable*>(skill);
    bool hasDefinedHotkey = keybindable && static_cast<bool>(keybindable->getHotkey());

    if (hasDefinedHotkey) {
      std::string&& hotkey = keycode_util::keyCodeToString(keybindable->getHotkey());
      label->setString(label->getString() + " [" + hotkey + "]");
    }
  };
}


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
