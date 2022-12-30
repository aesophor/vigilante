// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_TEXT_FIELD_H_
#define VIGILANTE_TEXT_FIELD_H_

#include <functional>
#include <string>

#include <cocos2d.h>
#include <2d/CCLabel.h>
#include <ui/UILayout.h>

#include "input/InputManager.h"

namespace vigilante {

class TextField {
 public:
  TextField(const std::string& defaultText="");
  virtual ~TextField() = default;

  virtual void update(float delta);

  virtual const std::string& getString() const { return _buffer; }
  virtual void setString(const std::string& s);
  virtual void clear();

  inline bool isReceivingInput() const { return _isReceivingInput; }
  void setReceivingInput(bool receivingInput);

  inline void setOnSubmit(const std::function<void ()>& onSubmit) { _onSubmit = onSubmit; }
  inline void setOnDismiss(const std::function<void ()>& onDismiss) { _onDismiss = onDismiss; }
  inline void setExtraOnKeyPressed(const InputManager::OnKeyPressedEvLstnr& extraOnKeyPressed) {
    _extraOnKeyPressed = extraOnKeyPressed;
  }
  inline void setDismissKey(cocos2d::EventKeyboard::KeyCode dismissKey) {
    _dismissKey = dismissKey;
  }

  inline cocos2d::ui::Layout* getLayout() const { return _layout; }

 private:
  virtual void toggleCursor();

  cocos2d::ui::Layout* _layout;
  cocos2d::Label* _label;
  std::string _buffer;
  std::function<void ()> _onSubmit;
  std::function<void ()> _onDismiss;
  InputManager::OnKeyPressedEvLstnr _onKeyPressed;
  InputManager::OnKeyPressedEvLstnr _extraOnKeyPressed;

  // The keyCode of the key which will dismiss this TextField
  cocos2d::EventKeyboard::KeyCode _dismissKey;

  float _timer;
  bool _isReceivingInput;
  bool _isCursorVisible;
};

}  // namespace vigilante

#endif  // VIGILANTE_TEXT_FIELD_H_
