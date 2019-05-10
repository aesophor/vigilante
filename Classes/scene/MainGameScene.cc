#include "MainGameScene.h"

#include <string>

#include "SimpleAudioEngine.h"

#include "character/Player.h"
#include "gl/GLESRender.h"
#include "util/box2d/b2DebugRenderer.h"
#include "util/CameraUtil.h"
#include "util/CallbackUtil.h"
#include "util/Constants.h"

using std::string;
using std::unique_ptr;
using vigilante::kPPM;
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

  auto winSize = Director::getInstance()->getWinSize();
  log("winSize: w=%f h=%f", winSize.width, winSize.height);

  auto oc = Camera::create();
  _gameCam = oc;
  oc->setCameraFlag(CameraFlag::USER1);

  const Vec3& eyePosOld = getDefaultCamera()->getPosition3D();
  Vec3 eyePos = {eyePosOld.x, eyePosOld.y, eyePosOld.z};
  oc->setPosition3D(eyePos);
  oc->lookAt(eyePos);
  addChild(oc);

  // Initialize Vigilante's CallbackUtil.
  vigilante::callback_util::init(this);
  
  // Initialize GameMapManager.
  // b2World is created when GameMapManager's ctor is called.
  _gameMapManager = unique_ptr<GameMapManager>(GameMapManager::getInstance());
  _gameMapManager->load("Map/starting_point.tmx");

  _gameMapManager->getLayer()->setCameraMask((unsigned short) CameraFlag::USER1);
  addChild(_gameMapManager->getLayer());

  oc->initOrthographic(winSize.width, winSize.height, 1, 1000);
  float mapHeight = _gameMapManager->getMap()->getMapSize().height;
  float tileSize = _gameMapManager->getMap()->getTileSize().height;
  float camOffsetY = (400 - mapHeight * tileSize) / 2;
  oc->setPosition(0, 0);
  oc->setAnchorPoint({0.0, 0.0});

  schedule(schedule_selector(MainGameScene::update));

  // Initialize GameInputManager.
  // GameInputManager keep tracks of which keys are pressed.
  _gameInputManager = unique_ptr<GameInputManager>(GameInputManager::getInstance());
  _gameInputManager->activate(this);

  // Create b2DebugRenderer.
  _b2dr = unique_ptr<b2DebugRenderer>(b2DebugRenderer::create(getWorld()));
  _b2dr->setCameraMask((unsigned short) CameraFlag::USER1);
  _b2dr->retain(); // TODO: remove unique_ptr for _b2dr
  _b2DebugOn = true;
  addChild(_b2dr.get());

  return true;
}

void MainGameScene::update(float delta) {
  const float kFps = 60.0f;
  const int kVelocityIterations = 6;
  const int kPositionIterations = 2;
  getWorld()->Step(1 / kFps, kVelocityIterations, kPositionIterations);

  handleInput(delta);
  _gameMapManager->getPlayer()->update(delta);

  vigilante::camera_util::lerpToTarget(_gameCam, _gameMapManager->getPlayer()->getB2Body()->GetPosition());
  //vigilante::camera_util::boundCamera(_gameCam, _gameMapManager->getMap());
  _gameCam->update(delta);
}

void MainGameScene::handleInput(float delta) {
  auto player = _gameMapManager->getPlayer();

  // Toggle b2dr (b2DebugRenderer)
  if (_gameInputManager->isKeyJustPressed(EventKeyboard::KeyCode::KEY_0)) {
    if (_b2DebugOn) {
      removeChild(_gameMapManager->getLayer());
      cocos2d::log("[b2dr] is off. Ref=%d", _b2dr->getReferenceCount());
    } else {
      addChild(_gameMapManager->getLayer());
      cocos2d::log("[b2dr] is on. Ref=%d", _b2dr->getReferenceCount());
    }
    _b2DebugOn = !_b2DebugOn;
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
