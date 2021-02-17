// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_GAME_SCENE_H_
#define VIGILANTE_GAME_SCENE_H_

#include <memory>

#include <cocos2d.h>
#include <ui/UIImageView.h>
#include <Box2D/Box2D.h>
#include "Controllable.h"
#include "map/FxManager.h"
#include "map/GameMapManager.h"
#include "ui/WindowManager.h"
#include "ui/Shade.h"
#include "ui/hud/Hud.h"
#include "ui/console/Console.h"
#include "ui/control_hints/ControlHints.h"
#include "ui/dialogue/DialogueManager.h"
#include "ui/floating_damages/FloatingDamages.h"
#include "ui/notifications/Notifications.h"
#include "ui/pause_menu/PauseMenu.h"
#include "ui/quest_hints/QuestHints.h"
#include "util/box2d/b2DebugRenderer.h"

namespace vigilante {

class GameScene : public cocos2d::Scene, public Controllable {
 public:
  CREATE_FUNC(GameScene);
  virtual ~GameScene() = default;

  virtual bool init() override;  // cocos2d::Scene
  virtual void update(float delta) override;  // cocos2d::Scene
  virtual void handleInput() override;  // Controllable

  void startNewGame();
  void loadGame(const std::string& gameSaveFilePath);

 private:
  cocos2d::Camera* _gameCamera;
  cocos2d::Camera* _hudCamera;
  b2DebugRenderer* _b2dr;  // autorelease object


  // For singleton classes, use raw pointers here.
  // Otherwise, use smart pointers (prefer unique_ptr<>).
  Shade* _shade;
  Hud* _hud;
  Console* _console;
  PauseMenu* _pauseMenu;
  WindowManager* _windowManager;
  ControlHints *_controlHints;
  DialogueManager* _dialogueManager;
  FloatingDamages* _floatingDamages;
  QuestHints* _questHints;
  Notifications* _notifications;
  GameMapManager* _gameMapManager;
  FxManager* _fxManager;
};

}  // namespace vigilante

#endif  // VIGILANTE_GAME_SCENE_H_
