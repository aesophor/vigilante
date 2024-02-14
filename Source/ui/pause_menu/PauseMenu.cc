// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "PauseMenu.h"

#include "Assets.h"
#include "Audio.h"
#include "input/InputManager.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "ui/pause_menu/equipment/EquipmentPane.h"
#include "ui/pause_menu/inventory/InventoryPane.h"
#include "ui/pause_menu/option/OptionPane.h"
#include "ui/pause_menu/quest/QuestPane.h"
#include "ui/pause_menu/skill/SkillPane.h"
#include "util/Logger.h"

using namespace std;
using namespace vigilante::assets;
USING_NS_AX;

namespace vigilante {

namespace {

const Vec2 kHeaderPanePos{140, 280};
const Vec2 kStatsPanePos{50, 240};
const Vec2 kDialogPos{50, 30};
const Vec2 kMainPanePos{230, 240};  // InventoryPane, EquipmentPane... etc

}  // namespace

PauseMenu::PauseMenu()
    : _layer{Layer::create()},
      _background{ui::ImageView::create(string{kPauseMenuBg})},
      _headerPane{std::make_unique<HeaderPane>(this)},
      _statsPane{std::make_unique<StatsPane>(this)},
      _dialog{std::make_unique<PauseMenuDialog>(this)} {
  // Scale the bg image to fill the entire visible area.
  const auto visibleSize = Director::getInstance()->getVisibleSize();
  _background->setScaleX(visibleSize.width / _background->getContentSize().width);
  _background->setScaleY(visibleSize.height / _background->getContentSize().height);
  _background->setAnchorPoint({0, 0});
  _background->setPosition({0, 0});
  _layer->addChild(_background, 0);

  // Initialize header (Inventory/Equipment/Skills/Quest/Options).
  _headerPane->getLayout()->setPosition(kHeaderPanePos);
  _layer->addChild(_headerPane->getLayout());

  // Initialize StatsPane.
  _statsPane->getLayout()->setPosition(kStatsPanePos);
  _layer->addChild(_statsPane->getLayout());

  // Initialize PauseMenuDialog.
  _dialog->getLayout()->setPosition(kDialogPos);
  _dialog->setVisible(false);
  _layer->addChild(_dialog->getLayout());

  // Initialize Main Panes.
  initMainPane(0, make_unique<InventoryPane>(this));
  initMainPane(1, make_unique<EquipmentPane>(this));
  initMainPane(2, make_unique<SkillPane>(this));
  initMainPane(3, make_unique<QuestPane>(this));
  initMainPane(4, make_unique<OptionPane>(this));

  // Show inventory pane by default.
  _panes.front()->setVisible(true);

  // By default, the PauseMenu should be invisible.
  _layer->setVisible(false);
}

void PauseMenu::initMainPane(int index, unique_ptr<AbstractPane> pane) {
  pane->setPosition(kMainPanePos);
  pane->setVisible(false);
  _layer->addChild(pane->getLayout());

  _panes[index] = std::move(pane);
}

void PauseMenu::update() {
  if (!getPlayer()) {
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

    auto controlHints = SceneManager::the().getCurrentScene<GameScene>()->getControlHints();
    controlHints->switchToProfile(static_cast<ControlHints::Profile>(_headerPane->getCurrentIndex()));

  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_E)) {
    getCurrentPane()->setVisible(false);
    _headerPane->selectNext();
    getCurrentPane()->update();
    getCurrentPane()->setVisible(true);

    auto controlHints = SceneManager::the().getCurrentScene<GameScene>()->getControlHints();
    controlHints->switchToProfile(static_cast<ControlHints::Profile>(_headerPane->getCurrentIndex()));
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
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  return gmMgr->getPlayer();
}

AbstractPane* PauseMenu::getCurrentPane() const {
  return _panes[_headerPane->getCurrentIndex()].get();
}

void PauseMenu::setVisible(bool visible) {
  if (visible && !isVisible()) {
    _layer->setVisible(true);
    
    auto controlHints = SceneManager::the().getCurrentScene<GameScene>()->getControlHints();
    controlHints->pushProfile(static_cast<ControlHints::Profile>(_headerPane->getCurrentIndex()));

  } else if (!visible && isVisible()) {
    _layer->setVisible(false);
    
    auto controlHints = SceneManager::the().getCurrentScene<GameScene>()->getControlHints();
    controlHints->popProfile();
  }

  Audio::the().playSfx(kSfxOpenClosePauseMenu);
}

}  // namespace vigilante
