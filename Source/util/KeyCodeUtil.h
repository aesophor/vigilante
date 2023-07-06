// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_KEYCODE_UTIL_H_
#define VIGILANTE_KEYCODE_UTIL_H_

#include <string>

#include <axmol.h>

namespace vigilante::keycode_util {

void init();
std::string keyCodeToString(ax::EventKeyboard::KeyCode keyCode);
char keyCodeToAscii(ax::EventKeyboard::KeyCode keyCode, bool isCapsLocked, bool isShiftPressed);

}  // namespace vigilante::keycode_util

#endif  // VIGILANTE_KEYCODE_UTIL_H_
