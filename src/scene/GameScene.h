// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_GAME_SCENE_H_
#define VIGILANTE_GAME_SCENE_H_

#include <memory>
#include <string>

#include <Box2D/Box2D.h>
#include <cocos2d.h>
#include <ui/UIImageView.h>

#include "Controllable.h"
#include "input/InputManager.h"
#include "input/HotkeyManager.h"
#include "map/GameMapManager.h"
#include "map/FxManager.h"
#include "ui/WindowManager.h"
#include "ui/Shade.h"
#include "ui/console/Console.h"
#include "ui/control_hints/ControlHints.h"
#include "ui/dialogue/DialogueManager.h"
#include "ui/floating_damages/FloatingDamages.h"
#include "ui/hud/Hud.h"
#include "ui/notifications/Notifications.h"
#include "ui/pause_menu/PauseMenu.h"
#include "ui/quest_hints/QuestHints.h"
#include "util/box2d/b2DebugRenderer.h"

namespace vigilante {

class GameScene : public cocos2d::Scene, public Controllable {
 public:
  CREATE_FUNC(GameScene);
  virtual ~GameScene() override = default;

  virtual bool init() override;  // cocos2d::Scene
  virtual void update(float delta) override;  // cocos2d::Scene

  virtual void handleInput() override;  // Controllable

  void startNewGame();
  void loadGame(const std::string& gameSaveFilePath);
  void quit();

  inline bool isRunning() const { return _isRunning; }
  inline void setRunning(bool running) { _isRunning = running; }

  inline Shade* getShade() const { return _shade.get(); }
  inline Hud* getHud() const { return _hud.get(); }
  inline Console* getConsole() const { return _console.get(); }
  inline PauseMenu* getPauseMenu() const { return _pauseMenu.get(); }
  inline WindowManager* getWindowManager() const { return _windowManager.get(); }
  inline ControlHints* getControlHints() const { return _controlHints.get(); }
  inline DialogueManager* getDialogueManager() const { return _dialogueManager.get(); }
  inline FloatingDamages* getFloatingDamages() const { return _floatingDamages.get(); }
  inline QuestHints* getQuestHints() const { return _questHints.get(); }
  inline Notifications* getNotifications() const { return _notifications.get(); }
  inline GameMapManager* getGameMapManager() const { return _gameMapManager.get(); }
  inline FxManager* getFxManager() const { return _fxManager.get(); }
  inline HotkeyManager* getHotkeyManager() const { return _hotkeyManager.get(); }

 private:
  bool _isRunning;
  bool _isTerminating;

  cocos2d::Camera* _gameCamera;
  cocos2d::Camera* _hudCamera;
  b2DebugRenderer* _b2dr;  // autorelease object

  std::unique_ptr<HotkeyManager> _hotkeyManager;
  std::unique_ptr<Shade> _shade;
  std::unique_ptr<Hud> _hud;
  std::unique_ptr<Console> _console;
  std::unique_ptr<Notifications> _notifications;
  std::unique_ptr<QuestHints> _questHints;
  std::unique_ptr<FloatingDamages> _floatingDamages;
  std::unique_ptr<ControlHints> _controlHints;
  std::unique_ptr<DialogueManager> _dialogueManager;
  std::unique_ptr<WindowManager> _windowManager;
  std::unique_ptr<GameMapManager> _gameMapManager;
  std::unique_ptr<FxManager> _fxManager;
  std::unique_ptr<PauseMenu> _pauseMenu;
};

}  // namespace vigilante

#endif  // VIGILANTE_GAME_SCENE_H_
