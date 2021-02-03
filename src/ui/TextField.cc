// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "TextField.h"

#include "AssetManager.h"
#include "input/InputManager.h"
#include "util/KeyCodeUtil.h"

#define CURSOR_CHAR "|"
#define CURSOR_BLINK_INTERVAL 0.7f

#define DEFAULT_DISMISS_KEY EventKeyboard::KeyCode::KEY_ENTER

using std::string;
using std::function;
using cocos2d::Label;
using cocos2d::ui::Layout;
using cocos2d::Event;
using cocos2d::EventKeyboard;
using vigilante::asset_manager::kRegularFont;
using vigilante::asset_manager::kRegularFontSize;

namespace vigilante {

TextField::TextField(const string& defaultText)
    : _layout(Layout::create()),
      _label(Label::createWithTTF(CURSOR_CHAR, kRegularFont, kRegularFontSize)),
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

    auto inputMgr = InputManager::getInstance();
    char c = keycode_util::keyCodeToAscii(keyCode, inputMgr->isCapsLocked(), inputMgr->isShiftPressed());
    if (c != 0x00) {
      _buffer += c;
      _label->setString(_buffer + CURSOR_CHAR);
    }
  };

  if (!defaultText.empty()) {
    setString(defaultText);
  }
}


void TextField::update(float delta) {
  _timer += delta;

  if (_timer >= CURSOR_BLINK_INTERVAL) {
    _timer = 0;
    toggleCursor();
  }
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


void TextField::toggleCursor() {
  _label->setString((_isCursorVisible) ? _buffer : _buffer + CURSOR_CHAR);
  _isCursorVisible = !_isCursorVisible;
}


bool TextField::isReceivingInput() const {
  return _isReceivingInput;
}

void TextField::setReceivingInput(bool receivingInput) {
  // Avoid repeatedly setting/clearing the same _onKeyPressed event listener
  if (receivingInput && !_isReceivingInput) {
    InputManager::getInstance()->setSpecialOnKeyPressed(_onKeyPressed);
  } else if (!receivingInput && _isReceivingInput) {
    InputManager::getInstance()->clearSpecialOnKeyPressed();
  }

  _isReceivingInput = receivingInput;
}

void TextField::setOnSubmit(const function<void ()>& onSubmit) {
  _onSubmit = onSubmit;
}

void TextField::setOnDismiss(const function<void ()>& onDismiss) {
  _onDismiss = onDismiss;
}

void TextField::setExtraOnKeyPressed(const InputManager::OnKeyPressedEvLstnr& extraOnKeyPressed) {
  _extraOnKeyPressed = extraOnKeyPressed;
}


void TextField::setDismissKey(EventKeyboard::KeyCode dismissKey) {
  _dismissKey = dismissKey;
}


Layout* TextField::getLayout() const {
  return _layout;
}

}  // namespace vigilante
