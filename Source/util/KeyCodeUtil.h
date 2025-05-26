// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_UTIL_KEYCODE_UTIL_H_
#define REQUIEM_UTIL_KEYCODE_UTIL_H_

#include <string>

#include <axmol.h>

namespace requiem::keycode_util {

void init();
std::string keyCodeToString(const ax::EventKeyboard::KeyCode keyCode);
char keyCodeToAscii(const ax::EventKeyboard::KeyCode keyCode, const bool isCapsLocked, const bool isShiftPressed);

}  // namespace requiem::keycode_util

#endif  // REQUIEM_UTIL_KEYCODE_UTIL_H_
