// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_HOTKEY_MANAGER_H_
#define VIGILANTE_HOTKEY_MANAGER_H_

#include <array>
#include <functional>

#include <cocos2d.h>
#include "input/Keybindable.h"

namespace vigilante {

class PauseMenuDialog;

class HotkeyManager {
 public:
  static HotkeyManager* getInstance();
  virtual ~HotkeyManager() = default;

  enum BindableKeys {
    LEFT_SHIFT,
    LEFT_CTRL,
    X,
    C,
    V,
    SIZE
  };

  static const std::array<cocos2d::EventKeyboard::KeyCode, BindableKeys::SIZE> _kBindableKeys;

  Keybindable* getHotkeyAction(cocos2d::EventKeyboard::KeyCode keyCode) const;
  void setHotkeyAction(cocos2d::EventKeyboard::KeyCode keyCode, Keybindable* keybindable);
  void clearHotkeyAction(cocos2d::EventKeyboard::KeyCode keyCode);
  void promptHotkey(Keybindable* keybindable, PauseMenuDialog* pauseMenuDialog);

 private:
  HotkeyManager();

  std::array<Keybindable*, BindableKeys::SIZE> _hotkeys;
};

} // namespace vigilante

#endif // VIGILANTE_HOTKEY_MANAGER
