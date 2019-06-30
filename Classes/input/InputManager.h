#ifndef VIGILANTE_INPUT_MANAGER_H_
#define VIGILANTE_INPUT_MANAGER_H_

#include <set>
#include <array>

#include "cocos2d.h"

#include "input/Keybindable.h"

namespace vigilante {

class InputManager {
 public:
  static InputManager* getInstance();
  virtual ~InputManager() = default;

  void activate(cocos2d::Scene* scene);
  void deactivate();

  bool isKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode) const;
  bool isKeyJustPressed(cocos2d::EventKeyboard::KeyCode keyCode);

  enum Hotkey {
    LEFT_SHIFT,
    LEFT_CTRL,
    X,
    C,
    V,
    SIZE
  };
  static const std::array<cocos2d::EventKeyboard::KeyCode, InputManager::Hotkey::SIZE> _kBindableKeys;

  Keybindable* getHotkeyAction(cocos2d::EventKeyboard::KeyCode keyCode) const;
  void setHotkeyAction(cocos2d::EventKeyboard::KeyCode keyCode, Keybindable* keybindable);

 private:
  static InputManager* _instance;
  InputManager();

  // Used by isKeyPressed(), isKeyJustPressed()
  cocos2d::Scene* _scene;
  cocos2d::EventListenerKeyboard* _keyboardEvLstnr;
  std::set<cocos2d::EventKeyboard::KeyCode> _pressedKeys;

  // Hotkeys (keybindings)
  std::array<Keybindable*, InputManager::Hotkey::SIZE> _hotkeys;
};

} // namespace vigilante

#endif // VIGILANTE_INPUT_MANAGER_H_
