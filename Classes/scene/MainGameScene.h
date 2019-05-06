#ifndef MAIN_GAME_SCENE_H_
#define MAIN_GAME_SCENE_H_

#include "cocos2d.h"
#include "Box2D/Box2D.h"

class MainGameScene : public cocos2d::Scene {
 public:
  virtual bool init() override;
  virtual void update(float delta) override;

  // A selector callback
  void menuCloseCallback(cocos2d::Ref* pSender);

  // Implement the "static create()" method manually
  CREATE_FUNC(MainGameScene);

  void createFixtures(cocos2d::CCTMXLayer* layer);
  void createRectangle(cocos2d::TMXLayer* layer, int x, int y, float width, float height);

 private:
  b2World* world_;
  cocos2d::TMXTiledMap* map_;
};

#endif // MAIN_GAME_SCENE_H_
