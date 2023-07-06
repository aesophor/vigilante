// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_KEYBINDABLE_H_
#define VIGILANTE_KEYBINDABLE_H_

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

#endif // VIGILANTE_KEYBINDABLE_H_
