#ifndef VIGILANTE_MAIN_GAME_SCENE_H_
#define VIGILANTE_MAIN_GAME_SCENE_H_

#include <memory>

#include "cocos2d.h"
#include "ui/UIImageView.h"
#include "Box2D/Box2D.h"

#include "map/GameMapManager.h"
#include "input/GameInputManager.h"
#include "ui/hud/Hud.h"
#include "ui/damage/FloatingDamageManager.h"
#include "ui/notification/NotificationManager.h"
#include "ui/pause_menu/PauseMenu.h"
#include "util/box2d/b2DebugRenderer.h"

class MainGameScene : public cocos2d::Scene {
 public:
  CREATE_FUNC(MainGameScene);
  virtual ~MainGameScene();

  virtual bool init() override;
  virtual void update(float delta) override;

  void handleInput(float delta);
  void pauseGame();
  void resumeGame();

  b2World* getWorld() const;

 private:
  cocos2d::Camera* _gameCamera;
  cocos2d::Camera* _hudCamera;
  cocos2d::ui::ImageView* _shade; // used for screen fade during game map transitions.

  bool _b2DebugOn;
  b2DebugRenderer* _b2dr; // Autorelease object

  bool _isPaused;
  std::unique_ptr<vigilante::PauseMenu> _pauseMenu;

  std::unique_ptr<vigilante::Hud> _hud;
  std::unique_ptr<vigilante::FloatingDamageManager> _floatingDamages;
  std::unique_ptr<vigilante::NotificationManager> _notifications;
  std::unique_ptr<vigilante::GameMapManager> _gameMapManager;
  std::unique_ptr<vigilante::GameInputManager> _gameInputManager;
};

#endif // VIGILANTE_MAIN_GAME_SCENE_H_
