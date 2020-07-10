// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_INPUT_MANAGER_H_
#define VIGILANTE_INPUT_MANAGER_H_

#include <set>
#include <stack>
#include <functional>

#include <cocos2d.h>
#include "input/Keybindable.h"

namespace vigilante {

class PauseMenuDialog;

class InputManager {
 public:
  static InputManager* getInstance();
  virtual ~InputManager() = default;

  void activate(cocos2d::Scene* scene);
  void deactivate();

  bool isKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode) const;
  bool isKeyJustPressed(cocos2d::EventKeyboard::KeyCode keyCode);

  using OnKeyPressedEvLstnr = std::function<void (cocos2d::EventKeyboard::KeyCode, cocos2d::Event*)>;
  void pushEvLstnr(const OnKeyPressedEvLstnr& evLstnr);
  void popEvLstnr();

  bool isCapsLocked() const;
  bool isShiftPressed() const;

 private:
  InputManager();

  cocos2d::Scene* _scene;
  cocos2d::EventListenerKeyboard* _keyboardEvLstnr;

  // Pressed Keys are stored in this set.
  // Relevant method: isKeyPressed(), isKeyJustPressed()
  std::set<cocos2d::EventKeyboard::KeyCode> _pressedKeys;

  // The functor at the top of the stack will be called whenever
  // an onKeyPressed Event arrives.
  // Relevant method: pushEvLstnr(), popEvLstnr()
  std::stack<OnKeyPressedEvLstnr> _onKeyPressedEvLstnrs;

  bool _isCapsLocked;
};

} // namespace vigilante

#endif // VIGILANTE_INPUT_MANAGER_H_
