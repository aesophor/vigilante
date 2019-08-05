// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "QuestPane.h"

#include "AssetManager.h"
#include "input/InputManager.h"
#include "map/GameMapManager.h"
#include "ui/pause_menu/PauseMenu.h"

using std::string;
using cocos2d::Label;
using cocos2d::ui::Layout;
using cocos2d::ui::ImageView;
using cocos2d::EventKeyboard;
using vigilante::asset_manager::kBoldFont;
using vigilante::asset_manager::kRegularFont;
using vigilante::asset_manager::kRegularFontSize;
using vigilante::asset_manager::kInventoryBg;
using vigilante::asset_manager::kTabRegular;
using vigilante::asset_manager::kTabHighlighted;

namespace vigilante {

QuestPane::QuestPane(PauseMenu* pauseMenu)
    : AbstractPane(pauseMenu),
      _background(ImageView::create(kInventoryBg)),
      _questListView(new QuestListView(pauseMenu)) {
  _background->setAnchorPoint({0, 1});

  _layout->setLayoutType(Layout::Type::ABSOLUTE);
  _layout->setAnchorPoint({0, 1}); // Make top-left (0, 0)
  _layout->addChild(_background);

  // Place item list view.
  _questListView->getLayout()->setPosition({5, -5});
  _layout->addChild(_questListView->getLayout());
}


void QuestPane::update() {
  _questListView->showQuests();
}

void QuestPane::handleInput() {
  auto inputMgr = InputManager::getInstance();

  if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_UP_ARROW)) {
    _questListView->selectUp();
  } else if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
    _questListView->selectDown();
  } else if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_ENTER)) {
    _questListView->confirm();
  }
}

} // namespace vigilante
