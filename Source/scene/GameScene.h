// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_SCENE_GAME_SCENE_H_
#define VIGILANTE_SCENE_GAME_SCENE_H_

#include <memory>
#include <string>

#include <axmol.h>
#include <extensions/axmol-ext.h>
#include <ui/UIImageView.h>

#include <box2d/box2d.h>

#include "AfterImageFxManager.h"
#include "Controllable.h"
#include "FxManager.h"
#include "input/HotkeyManager.h"
#include "input/InputManager.h"
#include "map/GameMapManager.h"
#include "ui/console/Console.h"
#include "ui/dialogue/DialogueManager.h"
#include "ui/hud/ControlHints.h"
#include "ui/hud/FloatingDamages.h"
#include "ui/hud/Hud.h"
#include "ui/hud/LocationInfo.h"
#include "ui/hud/Notifications.h"
#include "ui/pause_menu/PauseMenu.h"
#include "ui/quest_hints/QuestHints.h"
#include "ui/Shade.h"
#include "ui/WindowManager.h"

namespace vigilante {

class GameScene final : public ax::Scene, public Controllable {
 public:
  CREATE_FUNC(GameScene);
  virtual ~GameScene() override = default;

  virtual bool init() override;  // ax::Scene
  virtual void update(const float delta) override;  // ax::Scene

  virtual void handleInput() override;  // Controllable

  void startNewGame();
  void loadGame(const std::string& gameSaveFilePath);
  void quit();

  inline bool isRunning() const { return _isRunning; }
  inline void setRunning(bool running) { _isRunning = running; }

  inline ax::Camera* getGameCamera() const { return _gameCamera; }

  inline Shade* getShade() const { return _shade.get(); }
  inline Hud* getHud() const { return _hud.get(); }
  inline LocationInfo* getLocationInfo() const { return _locationInfo.get(); }
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
  inline AfterImageFxManager* getAfterImageFxManager() const { return _afterImageFxManager.get(); }
  inline HotkeyManager* getHotkeyManager() const { return _hotkeyManager.get(); }

 private:
  bool _isRunning;
  bool _isTerminating;

  ax::Camera* _parallaxCamera;
  ax::Camera* _gameCamera;
  ax::Camera* _hudCamera;
  ax::DrawNode* _drawBox2D;
  ax::extension::PhysicsDebugNodeBox2D _debugDraw;

  std::unique_ptr<HotkeyManager> _hotkeyManager;
  std::unique_ptr<Shade> _shade;
  std::unique_ptr<Hud> _hud;
  std::unique_ptr<LocationInfo> _locationInfo;
  std::unique_ptr<Console> _console;
  std::unique_ptr<Notifications> _notifications;
  std::unique_ptr<QuestHints> _questHints;
  std::unique_ptr<FloatingDamages> _floatingDamages;
  std::unique_ptr<ControlHints> _controlHints;
  std::unique_ptr<DialogueManager> _dialogueManager;
  std::unique_ptr<WindowManager> _windowManager;
  std::unique_ptr<GameMapManager> _gameMapManager;
  std::unique_ptr<FxManager> _fxManager;
  std::unique_ptr<AfterImageFxManager> _afterImageFxManager;
  std::unique_ptr<PauseMenu> _pauseMenu;
};

}  // namespace vigilante

#endif  // VIGILANTE_SCENE_GAME_SCENE_H_
