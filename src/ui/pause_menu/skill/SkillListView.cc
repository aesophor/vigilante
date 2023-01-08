// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "SkillListView.h"

#include <cassert>

#include "character/Player.h"
#include "input/Keybindable.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "ui/pause_menu/PauseMenu.h"
#include "ui/pause_menu/PauseMenuDialog.h"
#include "util/KeyCodeUtil.h"

#define VISIBLE_ITEM_COUNT 5
#define WIDTH 289.5
#define HEIGHT 120
#define ITEM_GAP_HEIGHT 25

#define DESC_LABEL_X 5
#define DESC_LABEL_Y -132

using namespace std;
using namespace vigilante::assets;
USING_NS_CC;

namespace vigilante {

SkillListView::SkillListView(PauseMenu* pauseMenu)
    : ListView<Skill*>(VISIBLE_ITEM_COUNT, WIDTH, HEIGHT, ITEM_GAP_HEIGHT, kItemRegular, kItemHighlighted),
      _pauseMenu(pauseMenu),
      _descLabel(Label::createWithTTF("", kRegularFont, kRegularFontSize)) {
  // _setObjectCallback is called at the end of ListView<T>::ListViewItem::setObject()
  // see ui/ListView.h
  _setObjectCallback = [](ListViewItem* listViewItem, Skill* skill) {
    assert(skill != nullptr);

    ui::ImageView* icon = listViewItem->getIcon();
    Label* label = listViewItem->getLabel();

    icon->loadTexture(skill->getIconPath());
    label->setString(skill->getName());

    // Display skill hotkey (if defined).
    // Skills are Keybindable, see skill/Skill.h
    Keybindable* keybindable = dynamic_cast<Keybindable*>(skill);
    bool hasDefinedHotkey = keybindable && static_cast<bool>(keybindable->getHotkey());

    if (hasDefinedHotkey) {
      const string& hotkey = keycode_util::keyCodeToString(keybindable->getHotkey());
      label->setString(label->getString() + " [" + hotkey + "]");
    }
  };

  _descLabel->getFontAtlas()->setAliasTexParameters();
  _descLabel->setAnchorPoint({0, 1});
  _descLabel->setPosition({DESC_LABEL_X, DESC_LABEL_Y});
  _descLabel->enableWrap(true);
  _layout->addChild(_descLabel);
}

void SkillListView::confirm() {
  Skill* skill = getSelectedObject();
  if (!skill) {
    return;
  }

  PauseMenuDialog* dialog = _pauseMenu->getDialog();
  dialog->reset();
  dialog->setMessage("What would you like to do with " + skill->getName() + "?");

  auto hotkeyMgr = SceneManager::the().getCurrentScene<GameScene>()->getHotkeyManager();
  dialog->setOption(0, true, "Assign", [=]() {
    dialog->reset();
    dialog->setMessage("Press a key to assign to...");
    dialog->setOption(2, true, "Cancel");
    dialog->show();
    hotkeyMgr->promptHotkey(skill, dialog);
  });

  if (static_cast<bool>(skill->getHotkey())) {
    dialog->setOption(1, true, "Clear", [=]() {
      hotkeyMgr->clearHotkeyAction(skill->getHotkey());
      //showSkills();
    });
  }

  dialog->setOption(2, true, "Cancel");
  dialog->show();
}

void SkillListView::selectUp() {
  ListView<Skill*>::selectUp();

  if (_objects.empty()) {
    return;
  }

  Skill* selectedSkill = getSelectedObject();
  _descLabel->setString((selectedSkill) ? selectedSkill->getDesc() : "");
}

void SkillListView::selectDown() {
  ListView<Skill*>::selectDown();

  if (_objects.empty()) {
    return;
  }

  Skill* selectedSkill = getSelectedObject();
  _descLabel->setString((selectedSkill) ? selectedSkill->getDesc() : "");
}

void SkillListView::showSkillsByType(Skill::Type skillType) {
  // Show player skills in SkillListView.
  setObjects(_pauseMenu->getPlayer()->getSkillBook()[skillType]);

  // Update description label.
  _descLabel->setString((_objects.size() > 0) ? _objects[_current]->getDesc() : "");
}

}  // namespace vigilante
