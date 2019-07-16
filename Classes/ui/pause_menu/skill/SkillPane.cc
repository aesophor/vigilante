// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "SkillPane.h"

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

SkillPane::SkillPane(PauseMenu* pauseMenu)
    : AbstractPane(pauseMenu),
      _background(ImageView::create(kInventoryBg)),
      _skillListView(new SkillListView(pauseMenu)) {
  _background->setAnchorPoint({0, 1});

  _layout->setLayoutType(Layout::Type::ABSOLUTE);
  _layout->setAnchorPoint({0, 1}); // Make top-left (0, 0)
  _layout->addChild(_background);

  // Place item list view.
  _layout->addChild(_skillListView->getLayout());
}


void SkillPane::update() {
  _skillListView->showSkills();
}

void SkillPane::handleInput() {
  auto inputMgr = InputManager::getInstance();

  if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_UP_ARROW)) {
    _skillListView->selectUp();
  } else if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
    _skillListView->selectDown();
  } else if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_ENTER)) {
    _skillListView->confirm();
  }
}

} // namespace vigilante
