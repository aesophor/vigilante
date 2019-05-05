#ifndef MAIN_GAME_SCENE_H_
#define MAIN_GAME_SCENE_H_

#include "cocos2d.h"

class MainGameScene : public cocos2d::Scene {
 public:
  virtual bool init();

  // A selector callback
  void menuCloseCallback(cocos2d::Ref* pSender);

  // Implement the "static create()" method manually
  CREATE_FUNC(MainGameScene);
};

#endif // MAIN_GAME_SCENE_H_
