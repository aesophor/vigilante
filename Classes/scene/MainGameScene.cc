#include "MainGameScene.h"

#include <string>

#include "SimpleAudioEngine.h"

#include "character/Player.h"
#include "util/box2d/b2DebugRenderer.h"
#include "util/CameraUtil.h"
#include "util/CallbackUtil.h"
#include "util/Constants.h"

using std::string;
using std::unique_ptr;
using vigilante::kFps;
using vigilante::kVelocityIterations;
using vigilante::kPositionIterations;
using vigilante::kPpm;
using vigilante::kGravity;
using vigilante::Player;
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
  
  // Initialize Vigilante's CallbackUtil.
  vigilante::callback_util::init(this);
  
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

  // Tick the box2d world.
  schedule(schedule_selector(MainGameScene::update));
  return true;
}

void MainGameScene::update(float delta) {
  getWorld()->Step(1 / kFps, kVelocityIterations, kPositionIterations);

  handleInput(delta);
  _gameMapManager->getPlayer()->update(delta);

  vigilante::camera_util::lerpToTarget(_gameCamera, _gameMapManager->getPlayer()->getB2Body()->GetPosition());
  vigilante::camera_util::boundCamera(_gameCamera, _gameMapManager->getMap());
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

  
  if (player->isAttacking() || player->isSheathingWeapon() || player->isUnsheathingWeapon()) {
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

  if (_gameInputManager->isKeyJustPressed(EventKeyboard::KeyCode::KEY_LEFT_ALT)) {
    player->jump();
  }

  if (player->isCrouching() && !_gameInputManager->isKeyPressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
    player->getUp();
  }
}


b2World* MainGameScene::getWorld() const {
  return _gameMapManager->getWorld();
}
