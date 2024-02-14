// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_INPUT_KEYBINDABLE_H_
#define VIGILANTE_INPUT_KEYBINDABLE_H_

#include <string>

#include <axmol.h>

namespace vigilante {

class Keybindable {
 public:
  virtual ~Keybindable() = default;

  virtual ax::EventKeyboard::KeyCode getHotkey() const = 0;
  virtual void setHotkey(ax::EventKeyboard::KeyCode hotkey) = 0;
};

} // namespace vigilante

#endif // VIGILANTE_INPUT_KEYBINDABLE_H_
