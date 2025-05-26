// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_UI_TEXT_FIELD_H_
#define REQUIEM_UI_TEXT_FIELD_H_

#include <functional>
#include <string>

#include <axmol.h>
#include <2d/Label.h>
#include <ui/UILayout.h>

#include "input/InputManager.h"

namespace requiem {

class TextField {
 public:
  TextField(const std::string& defaultText="");
  virtual ~TextField() = default;

  virtual void update(const float delta);

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
  inline void setDismissKey(ax::EventKeyboard::KeyCode dismissKey) {
    _dismissKey = dismissKey;
  }

  inline ax::ui::Layout* getLayout() const { return _layout; }

 private:
  virtual void toggleCursor();

  ax::ui::Layout* _layout;
  ax::Label* _label;
  std::string _buffer;
  std::function<void ()> _onSubmit;
  std::function<void ()> _onDismiss;
  InputManager::OnKeyPressedEvLstnr _onKeyPressed;
  InputManager::OnKeyPressedEvLstnr _extraOnKeyPressed;

  // The keyCode of the key which will dismiss this TextField
  ax::EventKeyboard::KeyCode _dismissKey;

  float _timer{};
  bool _isReceivingInput{};
  bool _isCursorVisible{};
};

}  // namespace requiem

#endif  // REQUIEM_UI_TEXT_FIELD_H_
