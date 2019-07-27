// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_MAIN_GAME_SCENE_H_
#define VIGILANTE_MAIN_GAME_SCENE_H_

#include <memory>

#include <cocos2d.h>
#include <ui/UIImageView.h>
#include <Box2D/Box2D.h>
#include "Controllable.h"
#include "map/GameMapManager.h"
#include "ui/Shade.h"
#include "ui/hud/Hud.h"
#include "ui/dialogue/DialogueManager.h"
#include "ui/floating_damages/FloatingDamages.h"
#include "ui/notifications/Notifications.h"
#include "ui/pause_menu/PauseMenu.h"
#include "ui/quest_hints/QuestHints.h"
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

  std::unique_ptr<Shade> _shade;
  std::unique_ptr<Hud> _hud;
  std::unique_ptr<PauseMenu> _pauseMenu;
  std::unique_ptr<DialogueManager> _dialogueManager;
  std::unique_ptr<FloatingDamages> _floatingDamages;
  std::unique_ptr<QuestHints> _questHints;
  std::unique_ptr<Notifications> _notifications;
  std::unique_ptr<GameMapManager> _gameMapManager;
};

} // namespace vigilante

#endif // VIGILANTE_MAIN_GAME_SCENE_H_
