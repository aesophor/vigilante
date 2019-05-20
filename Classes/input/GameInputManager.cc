#include "input/GameInputManager.h"

#include "map/GameMapManager.h"

using cocos2d::Scene;
using cocos2d::Event;
using cocos2d::EventKeyboard;
using cocos2d::EventListenerKeyboard;

namespace vigilante {

GameInputManager* GameInputManager::_instance = nullptr;

GameInputManager* GameInputManager::getInstance() {
  if (!_instance) {
    _instance = new GameInputManager();
  }
  return _instance;
}

GameInputManager::GameInputManager() : _scene(), _keyboardEvLstnr() {}


void GameInputManager::activate(Scene* scene) {
  _scene = scene;
  _keyboardEvLstnr = EventListenerKeyboard::create();

  _keyboardEvLstnr->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event) {
    if (_pressedKeys.find(keyCode) == _pressedKeys.end()) {
      _pressedKeys.insert(keyCode);
    }
  };

  _keyboardEvLstnr->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event) {
    _pressedKeys.erase(keyCode);
  };
  
  _scene->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_keyboardEvLstnr, scene);
}

void GameInputManager::deactivate() {
  _scene->getEventDispatcher()->removeEventListener(_keyboardEvLstnr);
  _keyboardEvLstnr = nullptr;
  _scene = nullptr;
}


bool GameInputManager::isKeyPressed(EventKeyboard::KeyCode keyCode) const {
  return _pressedKeys.find(keyCode) != _pressedKeys.end();
}

bool GameInputManager::isKeyJustPressed(EventKeyboard::KeyCode keyCode) {
  // std::set::erase() returns the number of elements erased.
  return static_cast<bool>(_pressedKeys.erase(keyCode));
}


} // namespace vigilante
