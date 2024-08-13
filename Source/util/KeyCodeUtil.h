// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_UTIL_KEYCODE_UTIL_H_
#define VIGILANTE_UTIL_KEYCODE_UTIL_H_

#include <string>

#include <axmol.h>

namespace vigilante::keycode_util {

void init();
std::string keyCodeToString(const ax::EventKeyboard::KeyCode keyCode);
char keyCodeToAscii(const ax::EventKeyboard::KeyCode keyCode, const bool isCapsLocked, const bool isShiftPressed);

}  // namespace vigilante::keycode_util

#endif  // VIGILANTE_UTIL_KEYCODE_UTIL_H_
