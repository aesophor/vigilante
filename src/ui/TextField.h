// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_TEXT_FIELD_H_
#define VIGILANTE_TEXT_FIELD_H_

#include <string>
#include <functional>

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

  virtual const std::string& getString() const;
  virtual void setString(const std::string& s);
  virtual void clear();

  bool isReceivingInput() const;
  void setReceivingInput(bool receivingInput);

  void setOnSubmit(const std::function<void ()>& onSubmit);
  void setOnDismiss(const std::function<void ()>& onDismiss);
  void setExtraOnKeyPressed(const InputManager::OnKeyPressedEvLstnr& extraOnKeyPressed);

  void setDismissKey(cocos2d::EventKeyboard::KeyCode dismissKey);

  cocos2d::ui::Layout* getLayout() const;

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
