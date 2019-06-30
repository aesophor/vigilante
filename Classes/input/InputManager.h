#ifndef VIGILANTE_INPUT_MANAGER_H_
#define VIGILANTE_INPUT_MANAGER_H_

#include <set>
#include <unordered_set>

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

  //Keybindable* getHotkeyAction(cocos2d::EventKeyboard::KeyCode keyCode) const;
  //void setHotkeyAction(cocos2d::EventKeyboard::KeyCode keyCode, Keybindable* keybindable);

 private:
  static InputManager* _instance;
  InputManager();

  // Used by isKeyPressed(), isKeyJustPressed()
  cocos2d::Scene* _scene;
  cocos2d::EventListenerKeyboard* _keyboardEvLstnr;
  std::set<cocos2d::EventKeyboard::KeyCode> _pressedKeys;

  // Hotkeys (keybindings)
  //std::unordered_set<cocos2d::EventKeyboard::KeyCode, Keybindable*> _hotkeys;
};

} // namespace vigilante

#endif // VIGILANTE_INPUT_MANAGER_H_
