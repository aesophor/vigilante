#ifndef VIGILANTE_GAME_INPUT_MANAGER_H_
#define VIGILANTE_GAME_INPUT_MANAGER_H_

#include <set>

#include "cocos2d.h"

namespace vigilante {

class GameInputManager {
 public:
  static GameInputManager* getInstance();
  virtual ~GameInputManager();

  void activate(cocos2d::Scene* scene);
  void deactivate();

  bool isKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode) const;
  bool isKeyJustPressed(cocos2d::EventKeyboard::KeyCode keyCode);

 private:
  static GameInputManager* _instance;
  GameInputManager();

  cocos2d::Scene* _scene;
  cocos2d::EventListenerKeyboard* _keyboardEvLstnr;
  std::set<cocos2d::EventKeyboard::KeyCode> _pressedKeys;
};

} // namespace vigilante

#endif // VIGILANTE_GAME_INPUT_MANAGER_H_
