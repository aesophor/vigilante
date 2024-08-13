// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "KeyCodeUtil.h"

#include <unordered_map>

using namespace std;
USING_NS_AX;

namespace {

// Apple's libc++ is retarded... This hash function object isn't
// necessary when compiling with libstdc++
#ifdef __APPLE__
struct KeyCodeHash {
  template <typename T>
  std::size_t operator()(T t) const {
    return static_cast<std::size_t>(t);
  }
};
unordered_map<EventKeyboard::KeyCode, string, KeyCodeHash> keyMap;
#else
unordered_map<EventKeyboard::KeyCode, string> keyMap;
#endif

}  // namespace

namespace vigilante::keycode_util {

void init() {
  ::keyMap[EventKeyboard::KeyCode::KEY_BACK] = "BACK";
  ::keyMap[EventKeyboard::KeyCode::KEY_BACKSPACE] = "BACKSPACE";
  ::keyMap[EventKeyboard::KeyCode::KEY_TAB] = "TAB";
  ::keyMap[EventKeyboard::KeyCode::KEY_BACK_TAB] = "BACKTAB";
  ::keyMap[EventKeyboard::KeyCode::KEY_RETURN] = "RETURN";
  ::keyMap[EventKeyboard::KeyCode::KEY_CAPS_LOCK] = "CAPS LOCK";
  ::keyMap[EventKeyboard::KeyCode::KEY_LEFT_SHIFT] = "LEFT SHIFT";
  ::keyMap[EventKeyboard::KeyCode::KEY_RIGHT_SHIFT] = "RIGHT SHIFT";
  ::keyMap[EventKeyboard::KeyCode::KEY_LEFT_CTRL] = "LEFT CTRL";
  ::keyMap[EventKeyboard::KeyCode::KEY_RIGHT_CTRL] = "RIGHT CTRL";
  ::keyMap[EventKeyboard::KeyCode::KEY_LEFT_ALT] = "LEFT ALT";
  ::keyMap[EventKeyboard::KeyCode::KEY_RIGHT_ALT] = "RIGHT ALT";
  ::keyMap[EventKeyboard::KeyCode::KEY_HOME] = "HOME";
  ::keyMap[EventKeyboard::KeyCode::KEY_PG_UP] = "PG_UP";
  ::keyMap[EventKeyboard::KeyCode::KEY_DELETE] = "DELETE";
  ::keyMap[EventKeyboard::KeyCode::KEY_END] = "END";
  ::keyMap[EventKeyboard::KeyCode::KEY_PG_DOWN] = "PG_DOWN";
  ::keyMap[EventKeyboard::KeyCode::KEY_LEFT_ARROW] = "LEFT ARROW";
  ::keyMap[EventKeyboard::KeyCode::KEY_RIGHT_ARROW] = "RIGHT ARROW";
  ::keyMap[EventKeyboard::KeyCode::KEY_UP_ARROW] = "UP ARROW";
  ::keyMap[EventKeyboard::KeyCode::KEY_DOWN_ARROW] = "DOWN ARROW";
  ::keyMap[EventKeyboard::KeyCode::KEY_PLUS] = "PLUS";
  ::keyMap[EventKeyboard::KeyCode::KEY_MINUS] = "MINUS";
  ::keyMap[EventKeyboard::KeyCode::KEY_ENTER] = "ENTER";
  ::keyMap[EventKeyboard::KeyCode::KEY_SPACE] = "SPACE";
  ::keyMap[EventKeyboard::KeyCode::KEY_SEMICOLON] = "SEMICOLON";
  ::keyMap[EventKeyboard::KeyCode::KEY_LEFT_BRACKET] = "LEFT BRACKET";
  ::keyMap[EventKeyboard::KeyCode::KEY_RIGHT_BRACKET] = "RIGHT BRACKET";

  ::keyMap[EventKeyboard::KeyCode::KEY_0] = "0";
  ::keyMap[EventKeyboard::KeyCode::KEY_1] = "1";
  ::keyMap[EventKeyboard::KeyCode::KEY_2] = "2";
  ::keyMap[EventKeyboard::KeyCode::KEY_3] = "3";
  ::keyMap[EventKeyboard::KeyCode::KEY_4] = "4";
  ::keyMap[EventKeyboard::KeyCode::KEY_5] = "5";
  ::keyMap[EventKeyboard::KeyCode::KEY_6] = "6";
  ::keyMap[EventKeyboard::KeyCode::KEY_7] = "7";
  ::keyMap[EventKeyboard::KeyCode::KEY_8] = "8";
  ::keyMap[EventKeyboard::KeyCode::KEY_9] = "9";

  ::keyMap[EventKeyboard::KeyCode::KEY_CAPITAL_A] = "A";
  ::keyMap[EventKeyboard::KeyCode::KEY_CAPITAL_B] = "B";
  ::keyMap[EventKeyboard::KeyCode::KEY_CAPITAL_C] = "C";
  ::keyMap[EventKeyboard::KeyCode::KEY_CAPITAL_D] = "D";
  ::keyMap[EventKeyboard::KeyCode::KEY_CAPITAL_E] = "E";
  ::keyMap[EventKeyboard::KeyCode::KEY_CAPITAL_F] = "F";
  ::keyMap[EventKeyboard::KeyCode::KEY_CAPITAL_G] = "G";
  ::keyMap[EventKeyboard::KeyCode::KEY_CAPITAL_H] = "H";
  ::keyMap[EventKeyboard::KeyCode::KEY_CAPITAL_I] = "I";
  ::keyMap[EventKeyboard::KeyCode::KEY_CAPITAL_J] = "J";
  ::keyMap[EventKeyboard::KeyCode::KEY_CAPITAL_K] = "K";
  ::keyMap[EventKeyboard::KeyCode::KEY_CAPITAL_L] = "L";
  ::keyMap[EventKeyboard::KeyCode::KEY_CAPITAL_M] = "M";
  ::keyMap[EventKeyboard::KeyCode::KEY_CAPITAL_N] = "N";
  ::keyMap[EventKeyboard::KeyCode::KEY_CAPITAL_O] = "O";
  ::keyMap[EventKeyboard::KeyCode::KEY_CAPITAL_P] = "P";
  ::keyMap[EventKeyboard::KeyCode::KEY_CAPITAL_Q] = "Q";
  ::keyMap[EventKeyboard::KeyCode::KEY_CAPITAL_R] = "R";
  ::keyMap[EventKeyboard::KeyCode::KEY_CAPITAL_S] = "S";
  ::keyMap[EventKeyboard::KeyCode::KEY_CAPITAL_T] = "T";
  ::keyMap[EventKeyboard::KeyCode::KEY_CAPITAL_U] = "U";
  ::keyMap[EventKeyboard::KeyCode::KEY_CAPITAL_V] = "V";
  ::keyMap[EventKeyboard::KeyCode::KEY_CAPITAL_W] = "W";
  ::keyMap[EventKeyboard::KeyCode::KEY_CAPITAL_X] = "X";
  ::keyMap[EventKeyboard::KeyCode::KEY_CAPITAL_Y] = "Y";
  ::keyMap[EventKeyboard::KeyCode::KEY_CAPITAL_Z] = "Z";

  ::keyMap[EventKeyboard::KeyCode::KEY_A] = "a";
  ::keyMap[EventKeyboard::KeyCode::KEY_B] = "b";
  ::keyMap[EventKeyboard::KeyCode::KEY_C] = "c";
  ::keyMap[EventKeyboard::KeyCode::KEY_D] = "d";
  ::keyMap[EventKeyboard::KeyCode::KEY_E] = "e";
  ::keyMap[EventKeyboard::KeyCode::KEY_F] = "f";
  ::keyMap[EventKeyboard::KeyCode::KEY_G] = "g";
  ::keyMap[EventKeyboard::KeyCode::KEY_H] = "h";
  ::keyMap[EventKeyboard::KeyCode::KEY_I] = "i";
  ::keyMap[EventKeyboard::KeyCode::KEY_J] = "j";
  ::keyMap[EventKeyboard::KeyCode::KEY_K] = "k";
  ::keyMap[EventKeyboard::KeyCode::KEY_L] = "l";
  ::keyMap[EventKeyboard::KeyCode::KEY_M] = "m";
  ::keyMap[EventKeyboard::KeyCode::KEY_N] = "n";
  ::keyMap[EventKeyboard::KeyCode::KEY_O] = "o";
  ::keyMap[EventKeyboard::KeyCode::KEY_P] = "p";
  ::keyMap[EventKeyboard::KeyCode::KEY_Q] = "q";
  ::keyMap[EventKeyboard::KeyCode::KEY_R] = "r";
  ::keyMap[EventKeyboard::KeyCode::KEY_S] = "s";
  ::keyMap[EventKeyboard::KeyCode::KEY_T] = "t";
  ::keyMap[EventKeyboard::KeyCode::KEY_U] = "u";
  ::keyMap[EventKeyboard::KeyCode::KEY_V] = "v";
  ::keyMap[EventKeyboard::KeyCode::KEY_W] = "w";
  ::keyMap[EventKeyboard::KeyCode::KEY_X] = "x";
  ::keyMap[EventKeyboard::KeyCode::KEY_Y] = "y";
  ::keyMap[EventKeyboard::KeyCode::KEY_Z] = "z";
}

string keyCodeToString(const EventKeyboard::KeyCode keyCode) {
  return ::keyMap[keyCode];
}

char keyCodeToAscii(const EventKeyboard::KeyCode keyCode, const bool isCapsLocked, const bool isShiftPressed) {
  if ((keyCode >= EventKeyboard::KeyCode::KEY_CAPITAL_A && keyCode <= EventKeyboard::KeyCode::KEY_CAPITAL_Z) ||
      (keyCode >= EventKeyboard::KeyCode::KEY_A && keyCode <= EventKeyboard::KeyCode::KEY_Z)) {
    char c = keyCodeToString(keyCode).front();

    if (isCapsLocked) {
      c -= 'a' - 'A';
    }

    if (isShiftPressed) {
      if (c >= 'A' && c <= 'Z') {
        c += 'a' - 'A';
      } else { // c >= 'a' && c <= 'z'
        c -= 'a' - 'A';
      }
    }

    return c;
  }

  const bool alt = isCapsLocked || isShiftPressed;
  switch (keyCode) {
    case EventKeyboard::KeyCode::KEY_1:
      return alt ? '!' : '1';
    case EventKeyboard::KeyCode::KEY_2:
      return alt ? '@' : '2';
    case EventKeyboard::KeyCode::KEY_3:
      return alt ? '#' : '3';
    case EventKeyboard::KeyCode::KEY_4:
      return alt ? '$' : '4';
    case EventKeyboard::KeyCode::KEY_5:
      return alt ? '%' : '5';
    case EventKeyboard::KeyCode::KEY_6:
      return alt ? '^' : '6';
    case EventKeyboard::KeyCode::KEY_7:
      return alt ? '&' : '7';
    case EventKeyboard::KeyCode::KEY_8:
      return alt ? '*' : '8';
    case EventKeyboard::KeyCode::KEY_9:
      return alt ? '(' : '9';
    case EventKeyboard::KeyCode::KEY_0:
      return alt ? ')' : '0';
    case EventKeyboard::KeyCode::KEY_MINUS:
      return alt ? '_' : '-';
    case EventKeyboard::KeyCode::KEY_EQUAL:
      return alt ? '+' : '=';
    case EventKeyboard::KeyCode::KEY_LEFT_BRACKET:
      return alt ? '{' : '[';
    case EventKeyboard::KeyCode::KEY_RIGHT_BRACKET:
      return alt ? '}' : ']';
    case EventKeyboard::KeyCode::KEY_COMMA:
      return alt ? '<' : ',';
    case EventKeyboard::KeyCode::KEY_PERIOD:
      return alt ? '>' : '.';
    case EventKeyboard::KeyCode::KEY_SLASH:
      return alt ? '?' : '/';
    case EventKeyboard::KeyCode::KEY_SEMICOLON:
      return alt ? ';' : ':';
    case EventKeyboard::KeyCode::KEY_SPACE:
      return ' ';
    default:
      return 0x00;
  }
}

}  // namespace vigilante::keycode_util
