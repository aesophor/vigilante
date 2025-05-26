// Copyright (c) 2018-2025 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_SCENE_GAME_SCENE_H_
#define REQUIEM_SCENE_GAME_SCENE_H_

#include <memory>
#include <string>

#include <axmol.h>
#include <extensions/axmol-ext.h>
#include <ui/UIImageView.h>

#include <box2d/box2d.h>

#include "AfterImageFxManager.h"
#include "Controllable.h"
#include "FxManager.h"
#include "gameplay/DialogueManager.h"
#include "gameplay/InGameTime.h"
#include "gameplay/RoomRentalTracker.h"
#include "input/HotkeyManager.h"
#include "input/InputManager.h"
#include "map/GameMapManager.h"
#include "ui/console/Console.h"
#include "ui/hud/ControlHints.h"
#include "ui/hud/FloatingDamages.h"
#include "ui/hud/Hud.h"
#include "ui/hud/TimeLocationInfo.h"
#include "ui/hud/Notifications.h"
#include "ui/pause_menu/PauseMenu.h"
#include "ui/quest_hints/QuestHints.h"
#include "ui/Shade.h"
#include "ui/WindowManager.h"

namespace requiem {

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

  inline bool isActive() const { return _isActive; }
  inline void setActive(bool active) { _isActive = active; }

  inline ax::Camera* getGameCamera() const { return _gameCamera; }

  inline HotkeyManager* getHotkeyManager() const { return _hotkeyManager.get(); }
  inline Shade* getShade() const { return _shade.get(); }
  inline Hud* getHud() const { return _hud.get(); }
  inline TimeLocationInfo* getTimeLocationInfo() const { return _timeLocationInfo.get(); }
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
  inline InGameTime* getInGameTime() const { return _inGameTime.get(); }
  inline RoomRentalTracker* getRoomRentalTracker() const { return _roomRentalTracker.get(); }

 private:
  bool _isActive;
  bool _isTerminating;

  ax::Camera* _parallaxCamera;
  ax::Camera* _gameCamera;
  ax::Camera* _hudCamera;
  ax::DrawNode* _drawBox2D;
  ax::extension::PhysicsDebugNodeBox2D _debugDraw;

  std::unique_ptr<HotkeyManager> _hotkeyManager;
  std::unique_ptr<Shade> _shade;
  std::unique_ptr<Hud> _hud;
  std::unique_ptr<TimeLocationInfo> _timeLocationInfo;
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
  std::unique_ptr<InGameTime> _inGameTime;
  std::unique_ptr<RoomRentalTracker> _roomRentalTracker;
};

}  // namespace requiem

#endif  // REQUIEM_SCENE_GAME_SCENE_H_
