// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_UTIL_KEYCODE_UTIL_H_
#define VIGILANTE_UTIL_KEYCODE_UTIL_H_

#include <string>

#include <axmol.h>

namespace vigilante::keycode_util {

void init();
std::string keyCodeToString(ax::EventKeyboard::KeyCode keyCode);
char keyCodeToAscii(ax::EventKeyboard::KeyCode keyCode, bool isCapsLocked, bool isShiftPressed);

}  // namespace vigilante::keycode_util

#endif  // VIGILANTE_UTIL_KEYCODE_UTIL_H_
