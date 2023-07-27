// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "TextField.h"

#include "Assets.h"
#include "input/InputManager.h"
#include "util/KeyCodeUtil.h"

#define CURSOR_CHAR "|"
#define CURSOR_BLINK_INTERVAL 0.7f

#define DEFAULT_DISMISS_KEY EventKeyboard::KeyCode::KEY_ENTER

using namespace std;
using namespace vigilante::assets;
USING_NS_AX;

namespace vigilante {

TextField::TextField(const string& defaultText)
    : _layout(ui::Layout::create()),
      _label(Label::createWithTTF(CURSOR_CHAR, string{kRegularFont}, kRegularFontSize)),
      _buffer(),
      _onSubmit(),
      _onDismiss(),
      _onKeyPressed(),
      _extraOnKeyPressed(),
      _dismissKey(DEFAULT_DISMISS_KEY),
      _timer(),
      _isReceivingInput(),
      _isCursorVisible() {

  _label->setAnchorPoint({0, 0});
  _layout->addChild(_label);

  _onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* e) {
    if (_extraOnKeyPressed) {
      _extraOnKeyPressed(keyCode, e);
    }

    if (keyCode == EventKeyboard::KeyCode::KEY_ENTER) {
      if (!_buffer.empty() && _onSubmit) {
        _onSubmit();
      }
    }

    if (keyCode == _dismissKey && _onDismiss) {
      _onDismiss();
      return;
    }

    if (keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE) {
      if (!_buffer.empty()) {
        _buffer.pop_back();
        _label->setString(_buffer + CURSOR_CHAR);
      }
      return;
    }

    char c = keycode_util::keyCodeToAscii(keyCode,
        InputManager::the().isCapsLocked(), InputManager::the().isShiftPressed());
    
    if (c != 0x00) {
      _buffer += c;
      _label->setString(_buffer + CURSOR_CHAR);
    }
  };

  if (!defaultText.empty()) {
    setString(defaultText);
  }
}

void TextField::update(const float delta) {
  _timer += delta;

  if (_timer >= CURSOR_BLINK_INTERVAL) {
    _timer = 0;
    toggleCursor();
  }
}

void TextField::setString(const string& s) {
  _buffer = s;
  _label->setString(_buffer + CURSOR_CHAR);
}

void TextField::clear() {
  setString("");
}

void TextField::toggleCursor() {
  _label->setString((_isCursorVisible) ? _buffer : _buffer + CURSOR_CHAR);
  _isCursorVisible = !_isCursorVisible;
}

void TextField::setReceivingInput(bool receivingInput) {
  // Avoid repeatedly setting/clearing the same _onKeyPressed event listener
  if (receivingInput && !_isReceivingInput) {
    InputManager::the().setSpecialOnKeyPressed(_onKeyPressed);
  } else if (!receivingInput && _isReceivingInput) {
    InputManager::the().clearSpecialOnKeyPressed();
  }

  _isReceivingInput = receivingInput;
}

}  // namespace vigilante
