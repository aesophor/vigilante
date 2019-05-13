#include "PauseMenu.h"

#include "input/GameInputManager.h"

using std::string;
using cocos2d::Director;
using cocos2d::Label;
using cocos2d::Layer;
using cocos2d::ui::ImageView;
using cocos2d::EventKeyboard;

namespace vigilante {

const string PauseMenu::_kPauseMenu = "Texture/UI/PauseMenu/";
const string PauseMenu::_kBackground = _kPauseMenu + "pause.png";
const string PauseMenu::_kStatsBg = _kPauseMenu + "stats_bg.png";
const string PauseMenu::_kFont = "Font/HeartbitXX2Px.ttf";
const float PauseMenu::_kFontSize = 16.0f;

PauseMenu::PauseMenu()
    : _layer(Layer::create()),
      _background(ImageView::create(_kBackground)),
      _headerPane(new HeaderPane()),
      _statsPane(new StatsPane()),
      _inventoryPane(new InventoryPane()) {
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

  // Initialize InventoryPane.
  _inventoryPane->getLayout()->setPosition({230, 240});
  _layer->addChild(_inventoryPane->getLayout());
}


void PauseMenu::handleInput() {
  GameInputManager* inputMgr = GameInputManager::getInstance();
  
  if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_Q)) {
    _headerPane->selectPrev();
  } else if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_E)) {
    _headerPane->selectNext();
  }
}


Layer* PauseMenu::getLayer() const {
  return _layer;
}

} // namespace vigilante
