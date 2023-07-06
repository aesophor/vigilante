// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_HOTKEY_MANAGER_H_
#define VIGILANTE_HOTKEY_MANAGER_H_

#include <array>
#include <functional>

#include <axmol.h>

#include "input/Keybindable.h"

namespace vigilante {

class PauseMenuDialog;

class HotkeyManager final {
 public:
  HotkeyManager();

  enum BindableKeys {
    LEFT_SHIFT,
    LEFT_CTRL,
    X,
    C,
    V,
    SIZE
  };

  static const std::array<ax::EventKeyboard::KeyCode, BindableKeys::SIZE> _kBindableKeys;

  Keybindable* getHotkeyAction(ax::EventKeyboard::KeyCode keyCode) const;
  void setHotkeyAction(ax::EventKeyboard::KeyCode keyCode, Keybindable* keybindable);
  void clearHotkeyAction(ax::EventKeyboard::KeyCode keyCode);
  void promptHotkey(Keybindable* keybindable, PauseMenuDialog* pauseMenuDialog);

 private:
  std::array<Keybindable*, BindableKeys::SIZE> _hotkeys;
};

} // namespace vigilante

#endif // VIGILANTE_HOTKEY_MANAGER
