#ifndef MAIN_GAME_SCENE_H_
#define MAIN_GAME_SCENE_H_

#include "cocos2d.h"

class HelloWorld : public cocos2d::Scene {
 public:
  virtual bool init();

  // A selector callback
  void menuCloseCallback(cocos2d::Ref* pSender);

  // Implement the "static create()" method manually
  CREATE_FUNC(HelloWorld);
};

#endif // MAIN_GAME_SCENE_H_
