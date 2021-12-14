// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "QuestPane.h"

#include "AssetManager.h"
#include "input/InputManager.h"
#include "util/Logger.h"

using cocos2d::ui::Layout;
using cocos2d::ui::ImageView;
using cocos2d::EventKeyboard;
using vigilante::asset_manager::kInventoryBg;
using vigilante::asset_manager::kTabRegular;
using vigilante::asset_manager::kTabHighlighted;

namespace vigilante {

QuestPane::QuestPane(PauseMenu* pauseMenu)
    : AbstractPane(pauseMenu),
      _background(ImageView::create(kInventoryBg)),
      _tabView(std::make_unique<TabView>(kTabRegular, kTabHighlighted)),
      _questListView(std::make_unique<QuestListView>(pauseMenu)) {
  _background->setAnchorPoint({0, 1});

  _layout->setLayoutType(Layout::Type::ABSOLUTE);
  _layout->setAnchorPoint({0, 1});  // Make top-left (0, 0)
  _layout->addChild(_background);

  // Place quest tabs.
  _tabView->addTab("ALL");
  _tabView->addTab("ACTIVE");
  _tabView->addTab("DONE");
  _tabView->selectTab(0);
  _layout->addChild(_tabView->getLayout());
  
  // Place item list view.
  _questListView->getLayout()->setPosition({5, -5});
  _layout->addChild(_questListView->getLayout());
}


void QuestPane::update() {
  switch (_tabView->getSelectedTab()->getIndex()) {
    case 0:
      _questListView->showAllQuests();
      break;
    case 1:
      _questListView->showInProgressQuests();
      break;
    case 2:
      _questListView->showCompletedQuests();
      break;
    default:
      VGLOG(LOG_ERR, "No such tab in QuestPane");
      break;
  }
}

void QuestPane::handleInput() {
  if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_LEFT_ARROW)) {
    _tabView->selectPrev();
    update();
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_RIGHT_ARROW)) {
    _tabView->selectNext();
    update();
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_UP_ARROW)) {
    _questListView->selectUp();
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
    _questListView->selectDown();
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_ENTER)) {
    _questListView->confirm();
  }
}

} // namespace vigilante
