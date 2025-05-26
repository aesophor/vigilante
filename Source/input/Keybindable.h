// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_INPUT_KEYBINDABLE_H_
#define REQUIEM_INPUT_KEYBINDABLE_H_

#include <string>

#include <axmol.h>

namespace requiem {

class Keybindable {
 public:
  virtual ~Keybindable() = default;

  virtual ax::EventKeyboard::KeyCode getHotkey() const = 0;
  virtual void setHotkey(ax::EventKeyboard::KeyCode hotkey) = 0;
};

} // namespace requiem

#endif // REQUIEM_INPUT_KEYBINDABLE_H_
