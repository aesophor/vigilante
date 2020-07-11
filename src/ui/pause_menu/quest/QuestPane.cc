// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "QuestPane.h"

#include "std/make_unique.h"
#include "AssetManager.h"
#include "input/InputManager.h"

using cocos2d::ui::Layout;
using cocos2d::ui::ImageView;
using cocos2d::EventKeyboard;

namespace vigilante {

QuestPane::QuestPane(PauseMenu* pauseMenu)
    : AbstractPane(pauseMenu),
      _background(ImageView::create(asset_manager::kInventoryBg)),
      _questListView(std::make_unique<QuestListView>(pauseMenu)) {
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
