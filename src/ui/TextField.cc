// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "TextField.h"

#include "AssetManager.h"
#include "input/InputManager.h"
#include "util/KeyCodeUtil.h"

#define CURSOR_CHAR "|"
#define CURSOR_BLINK_INTERVAL 0.7f

using std::string;
using std::function;
using cocos2d::Label;
using cocos2d::ui::Layout;
using cocos2d::Event;
using cocos2d::EventKeyboard;
using vigilante::asset_manager::kRegularFont;
using vigilante::asset_manager::kRegularFontSize;

namespace vigilante {

TextField::TextField()
    : _layout(Layout::create()),
      _label(Label::createWithTTF(CURSOR_CHAR, kRegularFont, kRegularFontSize)),
      _buffer(),
      _onSubmit(),
      _timer(),
      _isReceivingInput(),
      _isCursorVisible() {
  _label->setAnchorPoint({0, 0});
  _layout->addChild(_label);

  _onKeyPressedEvLstnr = [this](EventKeyboard::KeyCode keyCode, Event*) {
    if (keyCode == EventKeyboard::KeyCode::KEY_ENTER) {
      if (!_buffer.empty() && _onSubmit) {
        _onSubmit();
      }
      return;
    }

    if (keyCode == EventKeyboard::KeyCode::KEY_BACKSPACE) {
      if (!_buffer.empty()) {
        _buffer.pop_back();
        _label->setString(_buffer + CURSOR_CHAR);
      }
      return;
    }

    auto inputMgr = InputManager::getInstance();
    char c = keycode_util::keyCodeToAscii(keyCode, inputMgr->isCapsLocked(), inputMgr->isShiftPressed());
    if (c != 0x00) {
      _buffer += c;
      _label->setString(_buffer + CURSOR_CHAR);
    }
  };
}


void TextField::update(float delta) {
  _timer += delta;

  if (_timer >= CURSOR_BLINK_INTERVAL) {
    _timer = 0;
    toggleCursor();
  }
}

void TextField::handleInput() {

}


const string& TextField::getString() const {
  return _buffer;
}

void TextField::setString(const string& s) {
  _buffer = s;
  _label->setString(_buffer + CURSOR_CHAR);
}

void TextField::clear() {
  setString("");
}

void TextField::setOnSubmit(const function<void ()>& onSubmit) {
  _onSubmit = onSubmit;
}

void TextField::toggleCursor() {
  _label->setString((_isCursorVisible) ? _buffer : _buffer + CURSOR_CHAR);
  _isCursorVisible = !_isCursorVisible;
}


Layout* TextField::getLayout() const {
  return _layout;
}

bool TextField::isReceivingInput() const {
  return _isReceivingInput;
}

void TextField::setReceivingInput(bool receivingInput) {
  // Avoid repeatedly pushing/popping the same _onKeyPressedEvLstnr
  if (receivingInput && !_isReceivingInput) {
    InputManager::getInstance()->pushEvLstnr(_onKeyPressedEvLstnr);
  } else if (!receivingInput && _isReceivingInput) {
    InputManager::getInstance()->popEvLstnr();
  }

  _isReceivingInput = receivingInput;
}

} // namespace vigilante
