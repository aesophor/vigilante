// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "HotkeyManager.h"

#include "input/InputManager.h"
#include "ui/pause_menu/PauseMenuDialog.h"
#include "util/Logger.h"

using namespace std;
USING_NS_AX;

namespace vigilante {

Keybindable* HotkeyManager::getHotkeyAction(EventKeyboard::KeyCode keyCode) const {
  for (size_t i = 0; i < kBindableKeys.size(); i++) {
    if (keyCode == kBindableKeys[i]) {
      return _hotkeys[i];
    }
  }
  return nullptr;
}

void HotkeyManager::setHotkeyAction(EventKeyboard::KeyCode keyCode, Keybindable* keybindable) {
  for (size_t i = 0; i < kBindableKeys.size(); i++) {
    if (keyCode == kBindableKeys[i]) {
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

  for (size_t i = 0; i < kBindableKeys.size(); i++) {
    if (keyCode == kBindableKeys[i]) {
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

    // Everything done. Now it is safe to clear this functor.
    InputManager::the().clearSpecialOnKeyPressed();
  };

  InputManager::the().setSpecialOnKeyPressed(onKeyPressedEvLstnr);
}

}  // namespace vigilante
