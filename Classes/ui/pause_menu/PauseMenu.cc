#include "PauseMenu.h"

#include "GameAssetManager.h"
#include "map/GameMapManager.h"
#include "input/GameInputManager.h"
#include "ui/pause_menu/inventory/InventoryPane.h"
#include "ui/pause_menu/equipment/EquipmentPane.h"

using std::string;
using std::unique_ptr;
using cocos2d::Director;
using cocos2d::Label;
using cocos2d::Layer;
using cocos2d::ui::ImageView;
using cocos2d::EventKeyboard;
using vigilante::asset_manager::kPauseMenuBg;

namespace vigilante {

PauseMenu::PauseMenu()
    : _layer(Layer::create()),
      _background(ImageView::create(kPauseMenuBg)),
      _headerPane(new HeaderPane()),
      _statsPane(new StatsPane()) {
  // Scale the bg image to fill the entire visible area.
  auto visibleSize = Director::getInstance()->getVisibleSize();
  _background->setScaleX(visibleSize.width / _background->getContentSize().width);
  _background->setScaleY(visibleSize.height / _background->getContentSize().height);
  _background->setAnchorPoint({0, 0});
  _background->setPosition({0, 0});
  _layer->addChild(_background, 0);

  // Initialize header (Inventory/Equipment/Skills/Quest/Options).
  _headerPane->getLayout()->setPosition({100, 280});
  _layer->addChild(_headerPane->getLayout());

  // Initialize StatsPane.
  _statsPane->getLayout()->setPosition({50, 240});
  _layer->addChild(_statsPane->getLayout());

  // Initialize the size of _panes vector.
  _panes.resize(5);

  // Initialize InventoryPane.
  _panes[0] = unique_ptr<InventoryPane>(new InventoryPane());
  InventoryPane* inventoryPane = dynamic_cast<InventoryPane*>(_panes[0].get());
  inventoryPane->getLayout()->setPosition({230, 240});
  inventoryPane->setVisible(false);
  _layer->addChild(inventoryPane->getLayout());

  // Initialize EquipmentPane.
  Player* player = GameMapManager::getInstance()->getPlayer();
  _panes[1] = unique_ptr<EquipmentPane>(new EquipmentPane(player));
  EquipmentPane* equipmentPane = dynamic_cast<EquipmentPane*>(_panes[1].get());
  equipmentPane->getLayout()->setPosition({230, 240});
  equipmentPane->setVisible(false);
  _layer->addChild(equipmentPane->getLayout());

  // Show inventory pane by default.
  _panes.front()->setVisible(true);
}


void PauseMenu::update() {
  if (_panes[_headerPane->getCurrentIndex()]) {
    _panes[_headerPane->getCurrentIndex()]->update();
  }
}

void PauseMenu::handleInput() {
  GameInputManager* inputMgr = GameInputManager::getInstance();
  
  // FIXME: when all panes are implemented, this section of code
  // should be cleaned up.
  if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_Q)) {
    AbstractPane* oldPane = getCurrentPane();
    if (oldPane) {
      oldPane->setVisible(false);
    }
    _headerPane->selectPrev();
    AbstractPane* newPane = getCurrentPane();
    if (newPane) {
      newPane->setVisible(true);
    }
  } else if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_E)) {
    AbstractPane* oldPane = getCurrentPane();
    if (oldPane) {
      oldPane->setVisible(false);
    }
    _headerPane->selectNext();
    AbstractPane* newPane = getCurrentPane();
    if (newPane) {
      newPane->setVisible(true);
    }
  }

  if (_panes[_headerPane->getCurrentIndex()]) {
    _panes[_headerPane->getCurrentIndex()]->handleInput();
  }
}

AbstractPane* PauseMenu::getCurrentPane() const {
  return _panes[_headerPane->getCurrentIndex()].get();
}

Layer* PauseMenu::getLayer() const {
  return _layer;
}

} // namespace vigilante
