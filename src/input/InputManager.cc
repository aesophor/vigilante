// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "input/InputManager.h"

#include "util/Logger.h"

using std::set;
using cocos2d::Scene;
using cocos2d::Event;
using cocos2d::EventKeyboard;
using cocos2d::EventListenerKeyboard;

namespace vigilante {

InputManager* InputManager::getInstance() {
  static InputManager instance;
  return &instance;
}

InputManager::InputManager()
    : _scene(), _keyboardEvLstnr(), _pressedKeys(), _isCapsLocked() {}


void InputManager::activate(Scene* scene) {
  _scene = scene;
  _keyboardEvLstnr = EventListenerKeyboard::create();

  // Capture "this" by value.
  _keyboardEvLstnr->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* e) {
    _pressedKeys.insert(keyCode);

    if (keyCode == EventKeyboard::KeyCode::KEY_CAPS_LOCK) {
      _isCapsLocked = !_isCapsLocked;
    }

    // Execute additional OnKeyPressedEvLstnrs.
    if (!_onKeyPressedEvLstnrs.empty()) {
      _onKeyPressedEvLstnrs.top()(keyCode, e);
    }
  };

  _keyboardEvLstnr->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event*) {
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


void InputManager::pushEvLstnr(const OnKeyPressedEvLstnr& evLstnr) {
  _onKeyPressedEvLstnrs.push(evLstnr);
  VGLOG(LOG_INFO, "pushed. remaining evlstnrs: %ld", _onKeyPressedEvLstnrs.size());
}

void InputManager::popEvLstnr() {
  //VGLOG(LOG_INFO, "remaining evlstnrs: %ld", _onKeyPressedEvLstnrs.size());
  if (!_onKeyPressedEvLstnrs.empty()) {
    _onKeyPressedEvLstnrs.pop();
  }
  VGLOG(LOG_INFO, "popped. remaining evlstnrs: %ld", _onKeyPressedEvLstnrs.size());
}


bool InputManager::isCapsLocked() const {
  return _isCapsLocked;
}

bool InputManager::isShiftPressed() const {
  return isKeyPressed(EventKeyboard::KeyCode::KEY_SHIFT);
}

} // namespace vigilante
