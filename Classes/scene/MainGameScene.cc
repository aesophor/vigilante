#include "MainGameScene.h"

#include <string>

#include "SimpleAudioEngine.h"

#include "character/Player.h"
#include "util/box2d/b2DebugRenderer.h"
#include "util/Constants.h"
#include "util/CameraUtil.h"
#include "util/CallbackUtil.h"
#include "util/RandUtil.h"

using std::string;
using std::unique_ptr;
using vigilante::kFps;
using vigilante::kVelocityIterations;
using vigilante::kPositionIterations;
using vigilante::kPpm;
using vigilante::kGravity;
using vigilante::Player;
using vigilante::Hud;
using vigilante::PauseMenu;
using vigilante::Equipment;
using vigilante::GameMapManager;
using vigilante::GameInputManager;

USING_NS_CC;


MainGameScene::~MainGameScene() {}

bool MainGameScene::init() {
  if (!Scene::init()) {
    return false;
  }

  // Camera note:
  // DEFAULT (orthographic): used to render tilemaps/game objects
  // USER1 (orthographic): used to render HUD
  // Initialize the default camera from "perspective" to "orthographic",
  // and use it as the game world camera.
  auto winSize = Director::getInstance()->getWinSize();
  log("winSize: w=%f h=%f", winSize.width, winSize.height);
  _gameCamera = getDefaultCamera();
  _gameCamera->initOrthographic(winSize.width, winSize.height, 1, 1000);
  _gameCamera->setPosition(0, 0);

  // Initialize HUD camera.
  _hudCamera = Camera::createOrthographic(winSize.width, winSize.height, 1, 1000);
  _hudCamera->setDepth(2);
  _hudCamera->setCameraFlag(CameraFlag::USER1);
  const Vec3& eyePosOld = _gameCamera->getPosition3D();
  Vec3 eyePos = {eyePosOld.x, eyePosOld.y, eyePosOld.z};
  _hudCamera->setPosition3D(eyePos);
  _hudCamera->lookAt(eyePos);
  _hudCamera->setPosition(0, 0);
  addChild(_hudCamera);
  

  // Initialize HUD.
  _hud = unique_ptr<Hud>(Hud::getInstance());
  _hud->getLayer()->setCameraMask(static_cast<uint16_t>(CameraFlag::USER1));
  _hud->getLayer()->setPosition(75, winSize.height - 40);
  addChild(_hud->getLayer(), 90);

  // Initialize Vigilante's CallbackUtil.
  vigilante::callback_util::init(this);

  // Initialize Vigilante's RandUtil.
  vigilante::rand_util::init();
  
  // Initialize GameMapManager.
  // b2World is created when GameMapManager's ctor is called.
  _gameMapManager = unique_ptr<GameMapManager>(GameMapManager::getInstance());
  _gameMapManager->load("Map/starting_point.tmx");
  addChild(_gameMapManager->getLayer());

  // Initialize GameInputManager.
  // GameInputManager keep tracks of which keys are pressed.
  _gameInputManager = unique_ptr<GameInputManager>(GameInputManager::getInstance());
  _gameInputManager->activate(this);

  // Create b2DebugRenderer.
  _b2dr = b2DebugRenderer::create(getWorld());
  _b2dr->retain();
  _b2DebugOn = true;
  addChild(_b2dr);

  _hud->setPlayer(_gameMapManager->getPlayer());

  // Initialize Pause Menu.
  _pauseMenu = unique_ptr<PauseMenu>(new PauseMenu(_gameMapManager->getPlayer()));
  _pauseMenu->getLayer()->setCameraMask(static_cast<uint16_t>(CameraFlag::USER1));
  _pauseMenu->getLayer()->setVisible(false);
  _isPaused = false;
  addChild(_pauseMenu->getLayer(), 95);

  // Tick the box2d world.
  schedule(schedule_selector(MainGameScene::update));
  return true;
}

void MainGameScene::update(float delta) {
  if (_isPaused) {
    _pauseMenu->handleInput();

    // Pause/resume game.
    if (_gameInputManager->isKeyJustPressed(EventKeyboard::KeyCode::KEY_ESCAPE)) {
      if (!_isPaused) {
        pauseGame();
      } else {
        resumeGame();
      }
      _isPaused = !_isPaused;
      return;
    }
    return;
  }

  getWorld()->Step(1 / kFps, kVelocityIterations, kPositionIterations);
  handleInput(delta);

  for (auto character : _gameMapManager->getCharacters()) {
    character->update(delta);
  }
  for (auto item : _gameMapManager->getItems()) {
    item->update(delta);
  }

  vigilante::camera_util::lerpToTarget(_gameCamera, _gameMapManager->getPlayer()->getB2Body()->GetPosition());
  vigilante::camera_util::boundCamera(_gameCamera, _gameMapManager->getMap());
  vigilante::camera_util::updateShake(_gameCamera, delta);
}

void MainGameScene::handleInput(float delta) {
  auto player = _gameMapManager->getPlayer();

  // Toggle b2dr (b2DebugRenderer)
  if (_gameInputManager->isKeyJustPressed(EventKeyboard::KeyCode::KEY_0)) {
    if (_b2DebugOn) {
      removeChild(_b2dr);
      cocos2d::log("[b2dr] is off. Ref=%d", _b2dr->getReferenceCount());
    } else {
      addChild(_b2dr);
      cocos2d::log("[b2dr] is on. Ref=%d", _b2dr->getReferenceCount());
    }
    _b2DebugOn = !_b2DebugOn;
  }

  // Pause/resume game.
  if (_gameInputManager->isKeyJustPressed(EventKeyboard::KeyCode::KEY_ESCAPE)) {
    if (!_isPaused) {
      pauseGame();
    } else {
      resumeGame();
    }
    _isPaused = !_isPaused;
    return;
  }

  if (_isPaused) {
    _pauseMenu->handleInput();
  }
  
  if (player->isSetToKill() || player->isAttacking() || player->isSheathingWeapon() || player->isUnsheathingWeapon()) {
    return;
  }

  if (_gameInputManager->isKeyPressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
    player->crouch();
    if (_gameInputManager->isKeyJustPressed(EventKeyboard::KeyCode::KEY_LEFT_ALT)) {
      player->jumpDown();
    }
  }

  if (_gameInputManager->isKeyJustPressed(EventKeyboard::KeyCode::KEY_LEFT_CTRL)) {
    player->attack();
  }

  if (_gameInputManager->isKeyPressed(EventKeyboard::KeyCode::KEY_LEFT_ARROW)) {
    player->moveLeft();
  } else if (_gameInputManager->isKeyPressed(EventKeyboard::KeyCode::KEY_RIGHT_ARROW)) {
    player->moveRight();
  }

  if (_gameInputManager->isKeyJustPressed(EventKeyboard::KeyCode::KEY_R)) {
    if (player->isWeaponSheathed() && !player->isUnsheathingWeapon()) {
      player->unsheathWeapon();
    } else if (!player->isWeaponSheathed() && !player->isSheathingWeapon()) {
      player->sheathWeapon();
    }
  }

  if (_gameInputManager->isKeyJustPressed(EventKeyboard::KeyCode::KEY_Z)) {
    if (!player->getInRangeItems().empty()) {
      vigilante::Item* i = *(player->getInRangeItems().begin());
      player->addItem(i);
      i->getB2Body()->GetWorld()->DestroyBody(i->getB2Body());
      _gameMapManager->getItems().erase(i);
      _gameMapManager->getLayer()->removeChild(i->getSprite());

      //player->equip(dynamic_cast<Equipment*>(i)); // temporary!!!
    }
  }

  if (_gameInputManager->isKeyJustPressed(EventKeyboard::KeyCode::KEY_LEFT_ALT)) {
    player->jump();
  }

  if (player->isCrouching() && !_gameInputManager->isKeyPressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
    player->getUp();
  }
}


void MainGameScene::pauseGame() {
  _pauseMenu->update();
  _pauseMenu->getLayer()->setVisible(true);
}

void MainGameScene::resumeGame() {
  _pauseMenu->getLayer()->setVisible(false);
}


b2World* MainGameScene::getWorld() const {
  return _gameMapManager->getWorld();
}
