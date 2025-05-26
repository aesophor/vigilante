// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "input/InputManager.h"

#include "ui/TextField.h"

using namespace std;
USING_NS_AX;

namespace requiem {

InputManager& InputManager::the() {
  static InputManager instance;
  return instance;
}

void InputManager::activate(Scene* scene) {
  _scene = scene;
  _keyboardEvLstnr = EventListenerKeyboard::create();

  _keyboardEvLstnr->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* e) {
    if (keyCode == EventKeyboard::KeyCode::KEY_CAPS_LOCK) {
      _isCapsLocked = !_isCapsLocked;
    }

    _pressedKeys.insert(keyCode);

    if (_specialOnKeyPressed) {
      // Execute the additional onKeyPressed handler for special events.
      // (e.g., prompting for a hotkey, receiving TextField input, etc)
      bool shouldReleaseKey = false;
      _specialOnKeyPressed(keyCode, e, shouldReleaseKey);

      if (shouldReleaseKey) {
        _pressedKeys.erase(keyCode);
      }
    }
  };

  _keyboardEvLstnr->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event*) {
    _pressedKeys.erase(keyCode);
  };

  _scene->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_keyboardEvLstnr, scene);
}

void InputManager::deactivate() {
  if (!_scene) {
    return;
  }

  _scene->getEventDispatcher()->removeEventListener(_keyboardEvLstnr);
  _keyboardEvLstnr = nullptr;
  _scene = nullptr;
  _specialOnKeyPressed = nullptr;
  _pressedKeys.clear();
}

}  // namespace requiem
