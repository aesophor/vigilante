// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_MAIN_GAME_SCENE_H_
#define VIGILANTE_MAIN_GAME_SCENE_H_

#include <memory>

#include <cocos2d.h>
#include <ui/UIImageView.h>
#include <Box2D/Box2D.h>
#include "Controllable.h"
#include "map/GameMapManager.h"
#include "ui/Shade.h"
#include "ui/Window.h"
#include "ui/hud/Hud.h"
#include "ui/console/Console.h"
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
  virtual ~MainGameScene() = default;

  virtual bool init() override;  // cocos2d::Scene
  virtual void update(float delta) override;  // cocos2d::Scene
  virtual void handleInput() override;  // Controllable

  b2World* getWorld() const;

 private:
  cocos2d::Camera* _gameCamera;
  cocos2d::Camera* _hudCamera;
  b2DebugRenderer* _b2dr;  // autorelease object

  Shade* _shade;
  Hud* _hud;
  Console* _console;
  std::unique_ptr<PauseMenu> _pauseMenu;
  std::unique_ptr<Window> _window;
  DialogueManager* _dialogueManager;
  FloatingDamages* _floatingDamages;
  QuestHints* _questHints;
  Notifications* _notifications;
  GameMapManager* _gameMapManager;
};

}  // namespace vigilante

#endif  // VIGILANTE_MAIN_GAME_SCENE_H_
