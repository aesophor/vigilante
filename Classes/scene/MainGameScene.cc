#include "MainGameScene.h"

#include <string>

#include "SimpleAudioEngine.h"

#include "character/Player.h"
#include "gl/GLESRender.h"
#include "util/box2d/b2DebugRenderer.h"
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
  auto b2dr = b2DebugRenderer::create(getWorld());
  addChild(b2dr);

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

  if (_gameInputManager->isKeyPressed(EventKeyboard::KeyCode::KEY_A)) {
    player->moveLeft();
  } else if (_gameInputManager->isKeyPressed(EventKeyboard::KeyCode::KEY_D)) {
    player->moveRight();
  }

  if (_gameInputManager->isKeyJustPressed(EventKeyboard::KeyCode::KEY_W)) {
    player->jump();
  } else if (_gameInputManager->isKeyJustPressed(EventKeyboard::KeyCode::KEY_S)) {

  }
}


b2World* MainGameScene::getWorld() const {
  return _gameMapManager->getWorld();
}
