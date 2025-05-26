// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "TextField.h"

#include "Assets.h"
#include "input/InputManager.h"
#include "util/KeyCodeUtil.h"

using namespace std;
using namespace requiem::assets;
USING_NS_AX;

namespace requiem {

namespace {

constexpr char kCursor[] = "|";
constexpr float kCursorBlinkInterval = 0.7f;
constexpr auto kDefaultDismissKey = EventKeyboard::KeyCode::KEY_ENTER;

}  // namespace

TextField::TextField(const string& defaultText)
    : _layout{ui::Layout::create()},
      _label{Label::createWithTTF(kCursor, string{kRegularFont}, kRegularFontSize)},
      _dismissKey{kDefaultDismissKey} {
  _label->setAnchorPoint({0, 0});
  _layout->addChild(_label);

  _onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* e, bool& shouldReleaseKey) {
    if (_extraOnKeyPressed) {
      _extraOnKeyPressed(keyCode, e, shouldReleaseKey);
    }

    if (keyCode == EventKeyboard::KeyCode::KEY_ENTER) {
      if (!_buffer.empty() && _onSubmit) {
        _onSubmit();
      }
    }

    if (keyCode == _dismissKey && _onDismiss) {
      _onDismiss();
      shouldReleaseKey = true;
      return;
    }

    if (keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE) {
      if (!_buffer.empty()) {
        _buffer.pop_back();
        _label->setString(_buffer + kCursor);
      }
      return;
    }

    const char c = keycode_util::keyCodeToAscii(keyCode,
        InputManager::the().isCapsLocked(), InputManager::the().isShiftPressed());
    if (c) {
      _buffer += c;
      _label->setString(_buffer + kCursor);
    }
  };

  if (!defaultText.empty()) {
    setString(defaultText);
  }
}

void TextField::update(const float delta) {
  _timer += delta;

  if (_timer >= kCursorBlinkInterval) {
    _timer = 0;
    toggleCursor();
  }
}

void TextField::setString(const string& s) {
  _buffer = s;
  _label->setString(_buffer + kCursor);
}

void TextField::clear() {
  setString("");
}

void TextField::toggleCursor() {
  _label->setString((_isCursorVisible) ? _buffer : _buffer + kCursor);
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

}  // namespace requiem
