// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "PauseMenu.h"

#include "std/make_unique.h"
#include "AssetManager.h"
#include "input/InputManager.h"
#include "ui/pause_menu/inventory/InventoryPane.h"
#include "ui/pause_menu/equipment/EquipmentPane.h"
#include "ui/pause_menu/skill/SkillPane.h"
#include "ui/pause_menu/quest/QuestPane.h"
#include "ui/pause_menu/option/OptionPane.h"
#include "util/Logger.h"

#define HEADER_PANE_POS {140, 280}
#define STATS_PANE_POS {50, 240}
#define DIALOG_POS {50, 30}
#define MAIN_PANE_POS {230, 240} // InventoryPane, EquipmentPane... etc

using std::array;
using std::string;
using std::unique_ptr;
using cocos2d::Director;
using cocos2d::Label;
using cocos2d::Layer;
using cocos2d::ui::ImageView;
using cocos2d::EventKeyboard;
using vigilante::asset_manager::kPauseMenuBg;

namespace vigilante {

const array<string, PauseMenu::Pane::SIZE> PauseMenu::_kPaneNames = {
  "INVENTORY",
  "EQUIPMENT",
  "SKILLS",
  "QUESTS",
  "OPTIONS"
};

PauseMenu* PauseMenu::getInstance() {
  static PauseMenu instance;
  return &instance;
}

PauseMenu::PauseMenu()
    : _layer(Layer::create()),
      _background(ImageView::create(kPauseMenuBg)),
      _headerPane(std::make_unique<HeaderPane>(this)),
      _statsPane(std::make_unique<StatsPane>(this)),
      _dialog(std::make_unique<PauseMenuDialog>(this)),
      _player() {
  // Scale the bg image to fill the entire visible area.
  const auto visibleSize = Director::getInstance()->getVisibleSize();
  _background->setScaleX(visibleSize.width / _background->getContentSize().width);
  _background->setScaleY(visibleSize.height / _background->getContentSize().height);
  _background->setAnchorPoint({0, 0});
  _background->setPosition({0, 0});
  _layer->addChild(_background, 0);

  // Initialize header (Inventory/Equipment/Skills/Quest/Options).
  _headerPane->getLayout()->setPosition(HEADER_PANE_POS);
  _layer->addChild(_headerPane->getLayout());

  // Initialize StatsPane.
  _statsPane->getLayout()->setPosition(STATS_PANE_POS);
  _layer->addChild(_statsPane->getLayout());

  // Initialize PauseMenuDialog.
  _dialog->getLayout()->setPosition(DIALOG_POS);
  _dialog->setVisible(false);
  _layer->addChild(_dialog->getLayout());

  // Initialize Main Panes.
  initMainPane(0, std::make_unique<InventoryPane>(this));
  initMainPane(1, std::make_unique<EquipmentPane>(this));
  initMainPane(2, std::make_unique<SkillPane>(this));
  initMainPane(3, std::make_unique<QuestPane>(this));
  initMainPane(4, std::make_unique<OptionPane>(this));

  // Show inventory pane by default.
  _panes.front()->setVisible(true);

  // By default, the PauseMenu should be invisible.
  _layer->setVisible(false);
}

void PauseMenu::initMainPane(int index, std::unique_ptr<AbstractPane> pane) {
  pane->setPosition(MAIN_PANE_POS);
  pane->setVisible(false);
  _layer->addChild(pane->getLayout());

  _panes[index] = std::move(pane);
}


void PauseMenu::update() {
  if (!_player) {
    return;
  }

  _statsPane->update();
  _panes[_headerPane->getCurrentIndex()]->update();
}

void PauseMenu::handleInput() {
  if (_dialog->isVisible()) {
    _dialog->handleInput();
    return;
  }
  
  if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_Q)) {
    getCurrentPane()->setVisible(false);
    _headerPane->selectPrev();
    getCurrentPane()->update();
    getCurrentPane()->setVisible(true);

    ControlHints::getInstance()->switchToProfile(
        static_cast<ControlHints::Profile>(_headerPane->getCurrentIndex()));

  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_E)) {
    getCurrentPane()->setVisible(false);
    _headerPane->selectNext();
    getCurrentPane()->update();
    getCurrentPane()->setVisible(true);

    ControlHints::getInstance()->switchToProfile(
        static_cast<ControlHints::Profile>(_headerPane->getCurrentIndex()));
  }

  _panes[_headerPane->getCurrentIndex()]->handleInput();
}

void PauseMenu::show(Pane pane) {
  getCurrentPane()->setVisible(false);
  _headerPane->select(static_cast<int>(pane));
  getCurrentPane()->setVisible(true);
  update();
}


Player* PauseMenu::getPlayer() const {
  return _player;
}

void PauseMenu::setPlayer(Player* player) {
  _player = player;
}

AbstractPane* PauseMenu::getCurrentPane() const {
  return _panes[_headerPane->getCurrentIndex()].get();
}

Layer* PauseMenu::getLayer() const {
  return _layer;
}

PauseMenuDialog* PauseMenu::getDialog() const {
  return _dialog.get();
}


bool PauseMenu::isVisible() const {
  return _layer->isVisible();
}

void PauseMenu::setVisible(bool visible) {
  if (visible && !isVisible()) {
    _layer->setVisible(true);
    ControlHints::getInstance()->pushProfile(
        static_cast<ControlHints::Profile>(_headerPane->getCurrentIndex()));

  } else if (!visible && isVisible()) {
    _layer->setVisible(false);
    ControlHints::getInstance()->popProfile();
  }
}

}  // namespace vigilante
