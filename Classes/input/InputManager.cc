#include "input/InputManager.h"

#include "map/GameMapManager.h"

using cocos2d::Scene;
using cocos2d::Event;
using cocos2d::EventKeyboard;
using cocos2d::EventListenerKeyboard;

namespace vigilante {

InputManager* InputManager::_instance = nullptr;

InputManager* InputManager::getInstance() {
  if (!_instance) {
    _instance = new InputManager();
  }
  return _instance;
}

InputManager::InputManager() : _scene(), _keyboardEvLstnr() {}


void InputManager::activate(Scene* scene) {
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

void InputManager::deactivate() {
  _scene->getEventDispatcher()->removeEventListener(_keyboardEvLstnr);
  _keyboardEvLstnr = nullptr;
  _scene = nullptr;
}


bool InputManager::isKeyPressed(EventKeyboard::KeyCode keyCode) const {
  return _pressedKeys.find(keyCode) != _pressedKeys.end();
}

bool InputManager::isKeyJustPressed(EventKeyboard::KeyCode keyCode) {
  // std::set::erase() returns the number of elements erased.
  return _pressedKeys.erase(keyCode) > 0;
}


/*
Keybindable* InputManager::getHotkeyAction(EventKeyboard::KeyCode keyCode) const {

}

void InputManager::setHotkeyAction(EventKeyboard::KeyCode keyCode, Keybindable* keybindable) {
}
*/

} // namespace vigilante
