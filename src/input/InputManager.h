// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_INPUT_MANAGER_H_
#define VIGILANTE_INPUT_MANAGER_H_

#include <set>
#include <stack>
#include <functional>

#include <cocos2d.h>
#include "input/Keybindable.h"

#define IS_KEY_PRESSED(keyCode) \
  InputManager::getInstance()->isKeyPressed(keyCode)

#define IS_KEY_JUST_PRESSED(keyCode) \
  InputManager::getInstance()->isKeyJustPressed(keyCode)


namespace vigilante {

class InputManager {
 public:
  static InputManager* getInstance();
  virtual ~InputManager() = default;

  bool isActivated() const;
  void activate(cocos2d::Scene* scene);
  void deactivate();

  bool isKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode) const;
  bool isKeyJustPressed(cocos2d::EventKeyboard::KeyCode keyCode);

  bool isCapsLocked() const;
  bool isShiftPressed() const;


  using OnKeyPressedEvLstnr =
    std::function<void (cocos2d::EventKeyboard::KeyCode, cocos2d::Event*)>;

  bool hasSpecialOnKeyPressed() const;
  void setSpecialOnKeyPressed(const OnKeyPressedEvLstnr& onKeyPressed);
  void clearSpecialOnKeyPressed();

 private:
  InputManager();

  cocos2d::Scene* _scene;
  cocos2d::EventListenerKeyboard* _keyboardEvLstnr;

  bool _isCapsLocked;

  // Pressed Keys are stored in this set.
  // Relevant method: isKeyPressed(), isKeyJustPressed()
  std::set<cocos2d::EventKeyboard::KeyCode> _pressedKeys;

  OnKeyPressedEvLstnr _specialOnKeyPressed;
};

}  // namespace vigilante

#endif  // VIGILANTE_INPUT_MANAGER_H_
