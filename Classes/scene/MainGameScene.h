#ifndef VIGILANTE_MAIN_GAME_SCENE_H_
#define VIGILANTE_MAIN_GAME_SCENE_H_

#include <memory>

#include "cocos2d.h"
#include "ui/UIImageView.h"
#include "Box2D/Box2D.h"

#include "Controllable.h"
#include "map/GameMapManager.h"
#include "ui/hud/Hud.h"
#include "ui/damage/FloatingDamageManager.h"
#include "ui/notification/NotificationManager.h"
#include "ui/pause_menu/PauseMenu.h"
#include "util/box2d/b2DebugRenderer.h"

namespace vigilante {

class MainGameScene : public cocos2d::Scene, public Controllable {
 public:
  CREATE_FUNC(MainGameScene);
  virtual ~MainGameScene();

  virtual bool init() override; // cocos2d::Scene
  virtual void update(float delta) override; // cocos2d::Scene
  virtual void handleInput() override; // Controllable

  b2World* getWorld() const;

 private:
  cocos2d::Camera* _gameCamera;
  cocos2d::Camera* _hudCamera;
  b2DebugRenderer* _b2dr; // autorelease object
  cocos2d::ui::ImageView* _shade; // screen fade during map transitions

  std::unique_ptr<Hud> _hud;
  std::unique_ptr<PauseMenu> _pauseMenu;
  std::unique_ptr<FloatingDamageManager> _floatingDamages;
  std::unique_ptr<NotificationManager> _notifications;
  std::unique_ptr<GameMapManager> _gameMapManager;
};

} // namespace vigilante

#endif // VIGILANTE_MAIN_GAME_SCENE_H_
