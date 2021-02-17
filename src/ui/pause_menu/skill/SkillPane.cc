// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "SkillPane.h"

#include "std/make_unique.h"
#include "AssetManager.h"
#include "input/InputManager.h"

using cocos2d::ui::Layout;
using cocos2d::ui::ImageView;
using cocos2d::EventKeyboard;
using vigilante::asset_manager::kInventoryBg;
using vigilante::asset_manager::kTabRegular;
using vigilante::asset_manager::kTabHighlighted;

namespace vigilante {

SkillPane::SkillPane(PauseMenu* pauseMenu)
    : AbstractPane(pauseMenu),
      _background(ImageView::create(kInventoryBg)),
      _tabView(std::make_unique<TabView>(kTabRegular, kTabHighlighted)),
      _skillListView(std::make_unique<SkillListView>(pauseMenu)) {
  _background->setAnchorPoint({0, 1});

  _layout->setLayoutType(Layout::Type::ABSOLUTE);
  _layout->setAnchorPoint({0, 1}); // Make top-left (0, 0)
  _layout->addChild(_background);

  // Place skill category tabs.
  _tabView->addTab("MELEE");
  _tabView->addTab("MAGIC");
  _tabView->addTab("DARK");
  _tabView->addTab("BUFF");
  _tabView->selectTab(0);
  _layout->addChild(_tabView->getLayout());

  // Place item list view.
  _skillListView->getLayout()->setPosition({5, -5});
  _layout->addChild(_skillListView->getLayout());
}


void SkillPane::update() {
  Skill::Type selectedSkillType = static_cast<Skill::Type>(_tabView->getSelectedTab()->getIndex());
  _skillListView->showSkillsByType(selectedSkillType);
}

void SkillPane::handleInput() {
  if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_LEFT_ARROW)) {
    _tabView->selectPrev();
    update();
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_RIGHT_ARROW)) {
    _tabView->selectNext();
    update();
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_UP_ARROW)) {
    _skillListView->selectUp();
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
    _skillListView->selectDown();
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_ENTER)) {
    _skillListView->confirm();
  }
}

} // namespace vigilante
