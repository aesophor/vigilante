// Copyright (c) 2018-2025 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "HotkeyManager.h"

#include "input/InputManager.h"
#include "ui/pause_menu/PauseMenuDialog.h"
#include "util/Logger.h"

using namespace std;
USING_NS_AX;

namespace requiem {

shared_ptr<Keybindable> HotkeyManager::getHotkeyAction(EventKeyboard::KeyCode keyCode) const {
  for (size_t i = 0; i < kBindableKeys.size(); i++) {
    if (keyCode == kBindableKeys[i]) {
      return _hotkeys[i].lock();
    }
  }
  return nullptr;
}

void HotkeyManager::setHotkeyAction(EventKeyboard::KeyCode keyCode, shared_ptr<Keybindable> keybindable) {
  for (size_t i = 0; i < kBindableKeys.size(); i++) {
    if (keyCode != kBindableKeys[i]) {
      continue;
    }

    clearHotkeyAction(keybindable->getHotkey());
    if (auto registeredKeybindable = _hotkeys[i].lock()) {
      clearHotkeyAction(registeredKeybindable->getHotkey());
    }
    _hotkeys[i] = keybindable;
    keybindable->setHotkey(keyCode);
    return;
  }
}

void HotkeyManager::clearHotkeyAction(EventKeyboard::KeyCode keyCode) {
  // If keyCode == KeyCode::KEY_NONE, return at once.
  if (!static_cast<bool>(keyCode)) {
    return;
  }

  for (size_t i = 0; i < kBindableKeys.size(); i++) {
    if (keyCode != kBindableKeys[i]) {
      continue;
    }

    if (auto registeredKeybindable = _hotkeys[i].lock()) {
      registeredKeybindable->setHotkey(EventKeyboard::KeyCode::KEY_NONE);
    }
    _hotkeys[i].reset();
    return;
  }
}

void HotkeyManager::promptHotkey(shared_ptr<Keybindable> keybindable, PauseMenuDialog* pauseMenuDialog) {
  auto onKeyPressedEvLstnr = [this, keybindable, pauseMenuDialog](EventKeyboard::KeyCode keyCode, Event*, bool&) {
    setHotkeyAction(keyCode, keybindable);
    pauseMenuDialog->setVisible(false);
    pauseMenuDialog->getPauseMenu()->update();

    // Everything done. Now it is safe to clear this functor.
    InputManager::the().clearSpecialOnKeyPressed();
  };

  InputManager::the().setSpecialOnKeyPressed(onKeyPressedEvLstnr);
}

}  // namespace requiem
