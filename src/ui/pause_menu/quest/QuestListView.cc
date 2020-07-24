// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "QuestListView.h"

#include <cassert>

#include "character/Player.h"
#include "ui/pause_menu/PauseMenu.h"
#include "ui/pause_menu/PauseMenuDialog.h"

#define VISIBLE_ITEM_COUNT 5
#define WIDTH vigilante::kVirtualWidth / 2
#define LINE_HEIGHT 25
#define REGULAR_BG vigilante::asset_manager::kItemRegular
#define HIGHLIGHTED_BG vigilante::asset_manager::kItemHighlighted

#define DESC_LABEL_X 5
#define DESC_LABEL_Y -132

using cocos2d::Label;

namespace vigilante {

QuestListView::QuestListView(PauseMenu* pauseMenu)
    : ListView<Quest*>(VISIBLE_ITEM_COUNT, WIDTH, LINE_HEIGHT, REGULAR_BG, HIGHLIGHTED_BG),
      _pauseMenu(pauseMenu),
      _descLabel(Label::createWithTTF("", asset_manager::kRegularFont, asset_manager::kRegularFontSize)) {

  // _setObjectCallback is called at the end of ListView<T>::ListViewItem::setObject()
  // see ui/ListView.h
  _setObjectCallback = [](ListViewItem* listViewItem, Quest* quest) {
    assert(quest != nullptr);

    Label* label = listViewItem->getLabel();
    label->setString(quest->getQuestProfile().title);
  };

  _descLabel->getFontAtlas()->setAliasTexParameters();
  _descLabel->setAnchorPoint({0, 1});
  _descLabel->setPosition({DESC_LABEL_X, DESC_LABEL_Y});
  _descLabel->setWidth(WIDTH - DESC_LABEL_X * 2);
  _descLabel->enableWrap(true);
  _layout->addChild(_descLabel);
}


void QuestListView::confirm() {
  Quest* quest = getSelectedObject();
  if (!quest) {
    return;
  }

  /*
  PauseMenuDialog* dialog = _pauseMenu->getDialog();
  dialog->reset();
  dialog->setMessage("What would you like to do with " + skill->getName() + "?");

  dialog->setOption(0, true, "Assign", [=]() {
    dialog->reset();
    dialog->setMessage("Press a key to assign to...");
    dialog->setOption(2, true, "Cancel");
    dialog->show();
    HotkeyManager::getInstance()->promptHotkey(skill, dialog);
  });

  if (static_cast<bool>(skill->getHotkey())) {
    dialog->setOption(1, true, "Clear", [=]() {
      HotkeyManager::getInstance()->clearHotkeyAction(skill->getHotkey());
      showQuests();
    });
  }

  dialog->setOption(2, true, "Cancel");
  dialog->show();
  */
}


void QuestListView::selectUp() {
  ListView<Quest*>::selectUp();

  Quest* selectedQuest = getSelectedObject();
  _descLabel->setString((selectedQuest) ? selectedQuest->getQuestProfile().desc : "");
}

void QuestListView::selectDown() {
  ListView<Quest*>::selectDown();

  Quest* selectedQuest = getSelectedObject();
  _descLabel->setString((selectedQuest) ? selectedQuest->getQuestProfile().desc : "");
}


void QuestListView::showAllQuests() {
  // Show player skills in QuestListView.
  Player* player = _pauseMenu->getPlayer();
  setObjects(player->getQuestBook().getAllQuests());

  // Update description label.
  _descLabel->setString((_objects.size() > 0) ? _objects[_current]->getQuestProfile().desc : "");
}

void QuestListView::showInProgressQuests() {
  // Show player skills in QuestListView.
  Player* player = _pauseMenu->getPlayer();
  setObjects(player->getQuestBook().getInProgressQuests());

  // Update description label.
  _descLabel->setString((_objects.size() > 0) ? _objects[_current]->getQuestProfile().desc : "");
}

void QuestListView::showCompletedQuests() {
  // Show player skills in QuestListView.
  Player* player = _pauseMenu->getPlayer();
  setObjects(player->getQuestBook().getCompletedQuests());

  // Update description label.
  _descLabel->setString((_objects.size() > 0) ? _objects[_current]->getQuestProfile().desc : "");
}

}  // namespace vigilante
