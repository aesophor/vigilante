#ifndef VIGILANTE_MAIN_GAME_SCENE_H_
#define VIGILANTE_MAIN_GAME_SCENE_H_

#include "cocos2d.h"
#include "Box2D/Box2D.h"

#include "map/GameMapManager.h"

class MainGameScene : public cocos2d::Scene {
 public:
  // a macro which generates the "static create()" for us
  CREATE_FUNC(MainGameScene);
  virtual ~MainGameScene();

  virtual bool init() override;
  virtual void update(float delta) override;

  b2World* getWorld() const;

 private:
  vigilante::GameMapManager* _gameMapManager;
};

#endif // VIGILANTE_MAIN_GAME_SCENE_H_
