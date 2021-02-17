// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "input/InputManager.h"

#include "ui/TextField.h"
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
    : _scene(),
      _keyboardEvLstnr(),
      _isCapsLocked(),
      _pressedKeys(),
      _specialOnKeyPressed() {}


bool InputManager::isActivated() const {
  return _scene != nullptr;
}

void InputManager::activate(Scene* scene) {
  _scene = scene;
  _keyboardEvLstnr = EventListenerKeyboard::create();

  // Capture "this" by value.
  _keyboardEvLstnr->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* e) {
    if (keyCode == EventKeyboard::KeyCode::KEY_CAPS_LOCK) {
      _isCapsLocked = !_isCapsLocked;
    }

    if (!_specialOnKeyPressed) {
      // We keep track of which keys have been pressed
      // only when there is no active _specialOnKeyPressed event listener,
      // because _specialOnKeyPressed will do whatever it needs to do
      // with these keys.
      _pressedKeys.insert(keyCode);
    } else {
      // Execute the additional onKeyPressed handler for special events.
      // (e.g., prompting for a hotkey, receiving TextField input, etc)
      _specialOnKeyPressed(keyCode, e);
    }
  };

  _keyboardEvLstnr->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event*) {
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


bool InputManager::isCapsLocked() const {
  return _isCapsLocked;
}

bool InputManager::isShiftPressed() const {
  return isKeyPressed(EventKeyboard::KeyCode::KEY_SHIFT);
}


bool InputManager::hasSpecialOnKeyPressed() const {
  return static_cast<bool>(_specialOnKeyPressed);
}

void InputManager::setSpecialOnKeyPressed(const OnKeyPressedEvLstnr& onKeyPressed) {
  _specialOnKeyPressed = onKeyPressed;
}

void InputManager::clearSpecialOnKeyPressed() {
  _specialOnKeyPressed = nullptr;
}

}  // namespace vigilante
