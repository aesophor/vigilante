#include "MainGameScene.h"

#include <string>

#include "SimpleAudioEngine.h"

#include "character/Player.h"
#include "gl/GLESRender.h"
#include "util/box2d/b2DebugRenderer.h"
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
  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();
  log("visible size: %f %f\n", visibleSize.width, visibleSize.height);
  log("origin: %f %f\n", origin.x, origin.y);

  // Initialize Vigilante's CallbackUtil.
  vigilante::callback_util::init(this);
  
  // Initialize GameMapManager.
  // b2World is created when GameMapManager's ctor is called.
  _gameMapManager = unique_ptr<GameMapManager>(GameMapManager::getInstance());
  _gameMapManager->setScene(this);
  _gameMapManager->load("Map/starting_point.tmx");
  addChild(_gameMapManager->getMap(), 0);
  schedule(CC_SCHEDULE_SELECTOR(MainGameScene::update));

  // Initialize GameInputManager.
  // GameInputManager keep tracks of which keys are pressed.
  _gameInputManager = unique_ptr<GameInputManager>(GameInputManager::getInstance());
  _gameInputManager->activate(this);

  // Create b2DebugRenderer.
  _b2dr = unique_ptr<b2DebugRenderer>(b2DebugRenderer::create(getWorld()));
  _b2dr->retain();
  _b2DebugOn = true;
  addChild(_b2dr.get());

  // Zoom game camera.
  auto camPos = getDefaultCamera()->getPosition();
  getDefaultCamera()->setPosition(200, 100);
  getDefaultCamera()->setPositionZ(225);

  return true;
}

void MainGameScene::update(float delta) {
  const float kFps = 60.0f;
  const int kVelocityIterations = 6;
  const int kPositionIterations = 2;
  getWorld()->Step(1 / kFps, kVelocityIterations, kPositionIterations);

  handleInput(delta);
  _gameMapManager->getPlayer()->update(delta);
}

void MainGameScene::handleInput(float delta) {
  auto player = _gameMapManager->getPlayer();

  // Toggle b2dr (b2DebugRenderer)
  if (_gameInputManager->isKeyJustPressed(EventKeyboard::KeyCode::KEY_0)) {
    if (_b2DebugOn) {
      removeChild(_b2dr.get());
      cocos2d::log("[b2dr] is off. Ref=%d", _b2dr->getReferenceCount());
    } else {
      addChild(_b2dr.get());
      cocos2d::log("[b2dr] is on. Ref=%d", _b2dr->getReferenceCount());
    }
    _b2DebugOn = !_b2DebugOn;
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
  
  if (_gameInputManager->isKeyPressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
    player->crouch();
  }
}


b2World* MainGameScene::getWorld() const {
  return _gameMapManager->getWorld();
}
