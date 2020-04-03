// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_KEYCODE_UTIL_H_
#define VIGILANTE_KEYCODE_UTIL_H_

#include <string>

#include <cocos2d.h>

namespace vigilante {

namespace keycode_util {

void init();

std::string keyCodeToString(cocos2d::EventKeyboard::KeyCode keyCode);
char keyCodeToAscii(cocos2d::EventKeyboard::KeyCode keyCode, bool isCapsLocked,
                    bool isShiftPressed);

}  // namespace keycode_util

}  // namespace vigilante

#endif  // VIGILANTE_KEYCODE_UTIL_H_
