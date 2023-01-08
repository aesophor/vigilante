// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "input/InputManager.h"

#include "ui/TextField.h"
#include "util/Logger.h"

using namespace std;
USING_NS_CC;

namespace vigilante {

InputManager& InputManager::the() {
  static InputManager instance;
  return instance;
}

InputManager::InputManager()
    : _scene(),
      _keyboardEvLstnr(),
      _isCapsLocked(),
      _pressedKeys(),
      _specialOnKeyPressed() {}

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

}  // namespace vigilante
