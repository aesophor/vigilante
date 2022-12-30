// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_INPUT_MANAGER_H_
#define VIGILANTE_INPUT_MANAGER_H_

#include <functional>
#include <set>
#include <stack>

#include <cocos2d.h>

#include "input/Keybindable.h"

#define IS_KEY_PRESSED(keyCode) \
  InputManager::getInstance()->isKeyPressed(keyCode)

#define IS_KEY_JUST_PRESSED(keyCode) \
  InputManager::getInstance()->isKeyJustPressed(keyCode)

namespace vigilante {

class InputManager {
 public:
  using OnKeyPressedEvLstnr =
    std::function<void (cocos2d::EventKeyboard::KeyCode, cocos2d::Event*)>;

  static InputManager* getInstance();
  virtual ~InputManager() = default;

  void activate(cocos2d::Scene* scene);
  void deactivate();

  inline bool isActivated() const {
    return _scene != nullptr;
  }

  inline bool isKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode) const {
    return _pressedKeys.find(keyCode) != _pressedKeys.end();
  }

  inline bool isKeyJustPressed(cocos2d::EventKeyboard::KeyCode keyCode) {
    return _pressedKeys.erase(keyCode) > 0;
  }

  inline bool isCapsLocked() const {
    return _isCapsLocked;
  }

  inline bool isShiftPressed() const {
    return isKeyPressed(cocos2d::EventKeyboard::KeyCode::KEY_SHIFT);
  }

  inline bool hasSpecialOnKeyPressed() const {
    return static_cast<bool>(_specialOnKeyPressed);
  }

  inline void setSpecialOnKeyPressed(const OnKeyPressedEvLstnr& onKeyPressed) {
    _specialOnKeyPressed = onKeyPressed;
  }

  inline void clearSpecialOnKeyPressed() {
    _specialOnKeyPressed = nullptr;
  }

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
