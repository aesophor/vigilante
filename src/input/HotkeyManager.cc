// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "HotkeyManager.h"

#include "input/InputManager.h"
#include "ui/pause_menu/PauseMenuDialog.h"
#include "util/Logger.h"

using std::array;
using cocos2d::Event;
using cocos2d::EventKeyboard;

namespace vigilante {

HotkeyManager* HotkeyManager::_instance = nullptr;

const array<EventKeyboard::KeyCode, HotkeyManager::BindableKeys::SIZE> HotkeyManager::_kBindableKeys = {{
  EventKeyboard::KeyCode::KEY_LEFT_SHIFT,
  EventKeyboard::KeyCode::KEY_LEFT_CTRL,
  EventKeyboard::KeyCode::KEY_X,
  EventKeyboard::KeyCode::KEY_C,
  EventKeyboard::KeyCode::KEY_V
}};

HotkeyManager* HotkeyManager::getInstance() {
  if (!_instance) {
    _instance = new HotkeyManager();
  }
  return _instance;
}

HotkeyManager::HotkeyManager() : _hotkeys() {}


Keybindable* HotkeyManager::getHotkeyAction(EventKeyboard::KeyCode keyCode) const {
  for (size_t i = 0; i < _kBindableKeys.size(); i++) {
    if (keyCode == _kBindableKeys[i]) {
      return _hotkeys[i];
    }
  }
  return nullptr;
}

void HotkeyManager::setHotkeyAction(EventKeyboard::KeyCode keyCode, Keybindable* keybindable) {
  for (size_t i = 0; i < _kBindableKeys.size(); i++) {
    if (keyCode == _kBindableKeys[i]) {
      clearHotkeyAction(keybindable->getHotkey());
      if (_hotkeys[i]) {
        clearHotkeyAction(_hotkeys[i]->getHotkey());
      }

      _hotkeys[i] = keybindable;
      keybindable->setHotkey(keyCode);
      return;
    }
  }
}

void HotkeyManager::clearHotkeyAction(EventKeyboard::KeyCode keyCode) {
  // If keyCode == KeyCode::KEY_NONE, return at once.
  if (!static_cast<bool>(keyCode)) {
    return;
  }

  for (size_t i = 0; i < _kBindableKeys.size(); i++) {
    if (keyCode == _kBindableKeys[i]) {
      if (_hotkeys[i]) {
        _hotkeys[i]->setHotkey(EventKeyboard::KeyCode::KEY_NONE);
      }
      _hotkeys[i] = nullptr;
      return;
    }
  }
}

void HotkeyManager::promptHotkey(Keybindable* keybindable, PauseMenuDialog* pauseMenuDialog) {
  auto onKeyPressedEvLstnr = [=](EventKeyboard::KeyCode keyCode, Event*) {
    setHotkeyAction(keyCode, keybindable);
    pauseMenuDialog->setVisible(false);
    pauseMenuDialog->getPauseMenu()->update();
    
    // Everything done. Now it is safe to pop this functor off the stack.
    InputManager::getInstance()->popEvLstnr();
  };

  InputManager::getInstance()->pushEvLstnr(onKeyPressedEvLstnr);
}

} // namespace vigilante
