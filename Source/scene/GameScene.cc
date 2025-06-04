// Copyright (c) 2018-2025 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "GameScene.h"

#include "Assets.h"
#include "CallbackManager.h"
#include "Constants.h"
#include "character/Player.h"
#include "gameplay/ExpPointTable.h"
#include "gameplay/GameState.h"
#include "scene/SceneManager.h"
#include "skill/Skill.h"
#include "quest/Quest.h"
#include "util/CameraUtil.h"
#include "util/KeyCodeUtil.h"
#include "util/RandUtil.h"
#include "util/Logger.h"

using namespace std;
using namespace requiem::assets;
USING_NS_AX;

namespace requiem {

bool GameScene::init() {
  if (!Scene::init()) {
    return false;
  }

  _isActive = true;
  _isTerminating = false;

  // Initialize requiem's exp point table.
  exp_point_table::import(kExpPointTable);

  // Initialize InputManager.
  // InputManager keep tracks of which keys are pressed.
  InputManager::the().activate(this);

  // Initialize HotkeyManager.
  _hotkeyManager = std::make_unique<HotkeyManager>();

  // Initialize CallbackManager.
  CallbackManager::the().setScene(this);

  // Initialize requiem's utils.
  requiem::keycode_util::init();
  requiem::rand_util::init();

  // Camera note:
  // DEFAULT (orthographic): unused
  // USER1 (orthographic): used to render parallax background images
  // USER2 (orthographic): used to render tilemaps/game objects
  // USER3 (orthographic): used to render hud/ui elements
  const auto& winSize = Director::getInstance()->getWinSize();
  auto defaultCamera = getDefaultCamera();
  defaultCamera->initOrthographic(winSize.width, winSize.height, camera::kNearPlane, camera::kFarPlane);
  defaultCamera->setPosition(winSize.width / 2, winSize.height / 2);

  // Initialize Parallax camera.
  const Vec3& eyePosOld = defaultCamera->getPosition3D();
  const Vec3 eyePos = {eyePosOld.x, eyePosOld.y, eyePosOld.z};
  _parallaxCamera = Camera::createOrthographic(winSize.width, winSize.height, camera::kNearPlane, camera::kFarPlane);
  _parallaxCamera->setDepth(0);
  _parallaxCamera->setCameraFlag(camera::kParallaxCameraFlag);
  _parallaxCamera->setPosition3D(eyePos);
  _parallaxCamera->lookAt(eyePos);
  _parallaxCamera->setPosition(winSize / 2);
  addChild(_parallaxCamera);

  // Initialize game camera.
  _gameCamera = Camera::createOrthographic(winSize.width, winSize.height, camera::kNearPlane, camera::kFarPlane);
  _gameCamera->setDepth(1);
  _gameCamera->setCameraFlag(camera::kGameCameraFlag);
  _gameCamera->setPosition3D(eyePos);
  _gameCamera->lookAt(eyePos);
  _gameCamera->setPosition(winSize / 2);
  addChild(_gameCamera);

  // Initialize hud camera.
  _hudCamera = Camera::createOrthographic(winSize.width, winSize.height, camera::kNearPlane, camera::kFarPlane);
  _hudCamera->setDepth(2);
  _hudCamera->setCameraFlag(camera::kHudCameraFlag);
  _hudCamera->setPosition3D(eyePos);
  _hudCamera->lookAt(eyePos);
  _hudCamera->setPosition(winSize / 2);
  addChild(_hudCamera);

  // Initialize shade.
  _shade = std::make_unique<Shade>();
  _shade->getImageView()->setCameraMask(camera::kHudCameraMask);
  addChild(_shade->getImageView(), z_order::kShade);

  // Initialize HUD.
  _hud = std::make_unique<Hud>();
  _hud->getLayer()->setCameraMask(camera::kHudCameraMask);
  addChild(_hud->getLayer(), z_order::kHud);

  // Initialize TimeLocationInfo.
  _timeLocationInfo = std::make_unique<TimeLocationInfo>();
  _timeLocationInfo->getLayer()->setCameraMask(camera::kHudCameraMask);
  addChild(_timeLocationInfo->getLayer(), z_order::kTimeLocationInfo);

  // Initialize console.
  _console = std::make_unique<Console>();
  _console->getLayer()->setCameraMask(camera::kHudCameraMask);
  addChild(_console->getLayer(), z_order::kConsole);

  // Initialize notifications.
  _notifications = std::make_unique<Notifications>();
  _notifications->getLayer()->setCameraMask(camera::kHudCameraMask);
  addChild(_notifications->getLayer(), z_order::kNotification);

  // Initialize quest hints.
  _questHints = std::make_unique<QuestHints>();
  _questHints->getLayer()->setCameraMask(camera::kHudCameraMask);
  addChild(_questHints->getLayer(), z_order::kQuestHint);

  // Initialize floating damages.
  _floatingDamages = std::make_unique<FloatingDamages>();
  _floatingDamages->getLayer()->setCameraMask(camera::kGameCameraMask);
  addChild(_floatingDamages->getLayer(), z_order::kFloatingDamage);

  // Initialize control hints.
  _controlHints = std::make_unique<ControlHints>();
  _controlHints->getLayer()->setCameraMask(camera::kHudCameraMask);
  addChild(_controlHints->getLayer(), z_order::kControlHints);

  // Initialize dialogue manager.
  _dialogueManager = std::make_unique<DialogueManager>();
  _dialogueManager->getLayer()->setCameraMask(camera::kHudCameraMask);
  addChild(_dialogueManager->getLayer(), z_order::kDialogue);

  // Initialize window manager.
  _windowManager = std::make_unique<WindowManager>();
  _windowManager->setCameraMask(camera::kHudCameraMask);
  _windowManager->setScene(this);

  // Initialize GameMapManager.
  // b2World is created when GameMapManager's ctor is called.
  _gameMapManager = std::make_unique<GameMapManager>(b2Vec2{0, kGravity});
  _gameMapManager->getParallaxLayer()->setCameraMask(camera::kParallaxCameraMask);
  _gameMapManager->getLayer()->setCameraMask(camera::kGameCameraMask);
  addChild(_gameMapManager->getParallaxLayer());
  addChild(_gameMapManager->getLayer());

  // Initialize FxManager.
  _fxManager = std::make_unique<FxManager>();

  // Initialize AfterImageFxManager.
  _afterImageFxManager = std::make_unique<AfterImageFxManager>();

  // Initialize Pause Menu.
  _pauseMenu = std::make_unique<PauseMenu>();
  _pauseMenu->getLayer()->setCameraMask(camera::kHudCameraMask);
  addChild(_pauseMenu->getLayer(), z_order::kPauseMenu);

  // Initialize in-game time.
  _inGameTime = std::make_unique<InGameTime>();

  // Initialize room rental tracker.
  _roomRentalTracker = std::make_unique<RoomRentalTracker>();

  // Initialize box2d debug draw.
  uint32 flags = 0;
  flags += 1 * b2Draw::e_shapeBit;
  flags += 1 * b2Draw::e_jointBit;
  flags += 0 * b2Draw::e_aabbBit;
  flags += 1 * b2Draw::e_centerOfMassBit;
  _debugDraw.SetFlags(flags);
  _debugDraw.mRatio = kPpm;
  _debugDraw.debugNodeOffset = {0, 0};
  _gameMapManager->getWorld()->SetDebugDraw(&_debugDraw);
  _drawBox2D = _debugDraw.GetDrawNode();
  _drawBox2D->setOpacity(100);
  _drawBox2D->setCameraMask(camera::kGameCameraMask);
  _drawBox2D->setVisible(false);
  addChild(_drawBox2D);

  // Tick the box2d world.
  schedule(AX_SCHEDULE_SELECTOR(GameScene::update));

  return true;
}

void GameScene::update(const float delta) {
  if (!_isActive) {
    if (_isTerminating) {
      return;
    }
    _shade->getImageView()->runAction(Sequence::createWithTwoActions(
        FadeIn::create(Shade::kFadeInSec * 5),
        CallFunc::create([]() { SceneManager::the().popScene(); })
    ));
    _isTerminating = true;
    return;
  }

  if (!_gameMapManager->getGameMap()) {
    return;
  }

  handleInput();

  if (_pauseMenu->isVisible()) {
    return;
  }

  // If there are no ongoing GameMap transitions, then step the box2d world.
  if (_shade->getImageView()->getNumberOfRunningActions() == 0) {
    _gameMapManager->getWorld()->Step(1.0f / kFps, kVelocityIterations, kPositionIterations);
  }

  _inGameTime->update(delta);
  _timeLocationInfo->update();
  _gameMapManager->update(delta);
  _afterImageFxManager->update(delta);
  _floatingDamages->update(delta);
  _notifications->update(delta);
  _questHints->update(delta);
  _dialogueManager->update(delta);
  _console->update(delta);
  _windowManager->update(delta);

  if (_drawBox2D->isVisible()) {
    _drawBox2D->clear();
    _gameMapManager->getWorld()->DebugDraw();
  }

  camera_util::lerpToTarget(_gameCamera, _gameMapManager->getPlayer()->getBody()->GetPosition());
  camera_util::boundCamera(_gameCamera, _gameMapManager->getGameMap());
  camera_util::updateShake(_gameCamera, delta);
}

void GameScene::handleInput() {
  // First thing first: If there is a specialOnKeyPressed Event Listener, then we should simply
  // let it do its job, and return immediately so that we won't interfere with it.
  if (InputManager::the().hasSpecialOnKeyPressed()) {
    return;
  }

  if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_0)) {
    bool isVisible = !_drawBox2D->isVisible();
    _drawBox2D->setVisible(isVisible);
    _notifications->show(string("Debug Mode: ") + ((isVisible) ? "on" : "off"));
    return;
  }

  if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_ESCAPE)) {
    if (!_windowManager->isEmpty()) {
      _windowManager->pop();
      return;
    }
    if (_dialogueManager->getSubtitles()->isOngoing()) {
      return;
    }
    if (_gameMapManager->isLoadingGameMap()) {
      return;
    }

    bool isVisible = !_pauseMenu->isVisible();
    _pauseMenu->setVisible(isVisible);
    _pauseMenu->update();
    return;
  }

  if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_GRAVE)) {
    _console->setVisible(true);
    return;
  }

  if (!_windowManager->isEmpty()) {
    _windowManager->top()->handleInput();
    return;
  }

  if (_pauseMenu->isVisible()) {
    _pauseMenu->handleInput();
    return;
  }

  if (_dialogueManager->getDialogueMenu()->getLayer()->isVisible() ||
      _dialogueManager->getSubtitles()->getLayer()->isVisible()) {
    _dialogueManager->handleInput();
    return;
  }

  if (_gameMapManager->getPlayer()) {
    _gameMapManager->getPlayer()->handleInput();
  }
}

void GameScene::startNewGame() {
  _gameMapManager->loadGameMap(kNewGameInitialMap);
}

void GameScene::loadGame(const string& gameSaveFilePath) {
  _gameMapManager->loadGameMap(kNewGameInitialMap, [gameSaveFilePath](const GameMap*) {
    GameState(gameSaveFilePath).load();
  });
}

void GameScene::quit() {
  _gameMapManager->destroyGameMap();
}

}  // namespace requiem
