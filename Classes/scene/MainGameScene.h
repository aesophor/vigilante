#ifndef VIGILANTE_MAIN_GAME_SCENE_H_
#define VIGILANTE_MAIN_GAME_SCENE_H_

#include <memory>

#include "cocos2d.h"
#include "Box2D/Box2D.h"

#include "map/GameMapManager.h"
#include "input/GameInputManager.h"
#include "util/box2d/b2DebugRenderer.h"

class MainGameScene : public cocos2d::Scene {
 public:
  // a macro which generates the "static create()" for us
  CREATE_FUNC(MainGameScene);
  virtual ~MainGameScene();

  virtual bool init() override;
  virtual void update(float delta) override;
  virtual void handleInput(float delta);

  b2World* getWorld() const;

 private:
  bool _b2DebugOn;

  std::unique_ptr<b2DebugRenderer> _b2dr;
  std::unique_ptr<vigilante::GameMapManager> _gameMapManager;
  std::unique_ptr<vigilante::GameInputManager> _gameInputManager;
};

#endif // VIGILANTE_MAIN_GAME_SCENE_H_
