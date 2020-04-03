// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_KEYBINDABLE_H_
#define VIGILANTE_KEYBINDABLE_H_

#include <string>

#include <cocos2d.h>

namespace vigilante {

class Keybindable {
 public:
  virtual ~Keybindable() = default;

  virtual cocos2d::EventKeyboard::KeyCode getHotkey() const = 0;
  virtual void setHotkey(cocos2d::EventKeyboard::KeyCode hotkey) = 0;
};

}  // namespace vigilante

#endif  // VIGILANTE_KEYBINDABLE_H_
