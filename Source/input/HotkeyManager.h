// Copyright (c) 2018-2025 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_INPUT_HOTKEY_MANAGER_H_
#define REQUIEM_INPUT_HOTKEY_MANAGER_H_

#include <array>
#include <functional>
#include <memory>

#include <axmol.h>

#include "input/Keybindable.h"

namespace requiem {

class PauseMenuDialog;

class HotkeyManager final {
 public:
  friend class GameState;

  enum BindableKeys {
    LEFT_SHIFT,
    LEFT_CTRL,
    X,
    C,
    V,
    SIZE
  };

  static inline const std::array<ax::EventKeyboard::KeyCode, BindableKeys::SIZE> kBindableKeys{{
    ax::EventKeyboard::KeyCode::KEY_LEFT_SHIFT,
    ax::EventKeyboard::KeyCode::KEY_LEFT_CTRL,
    ax::EventKeyboard::KeyCode::KEY_X,
    ax::EventKeyboard::KeyCode::KEY_C,
    ax::EventKeyboard::KeyCode::KEY_V
  }};

  std::shared_ptr<Keybindable> getHotkeyAction(ax::EventKeyboard::KeyCode keyCode) const;
  void setHotkeyAction(ax::EventKeyboard::KeyCode keyCode, std::shared_ptr<Keybindable> keybindable);
  void clearHotkeyAction(ax::EventKeyboard::KeyCode keyCode);
  void promptHotkey(std::shared_ptr<Keybindable> keybindable, PauseMenuDialog* pauseMenuDialog);

 private:
  std::array<std::weak_ptr<Keybindable>, BindableKeys::SIZE> _hotkeys;
};

} // namespace requiem

#endif // REQUIEM_INPUT_HOTKEY_MANAGER_H_
