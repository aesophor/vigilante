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

  // Camera note:
  // DEFAULT (orthographic): used to render tilemaps/game objects
  // USER1 (orthographic): used to render HUD
  auto winSize = Director::getInstance()->getWinSize();
  log("winSize: w=%f h=%f", winSize.width, winSize.height);
 
  // Initialize the default camera from "perspective" to "orthographic",
  // and use it as the game world camera.
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

  schedule(schedule_selector(MainGameScene::update));

  // Initialize GameInputManager.
  // GameInputManager keep tracks of which keys are pressed.
  _gameInputManager = unique_ptr<GameInputManager>(GameInputManager::getInstance());
  _gameInputManager->activate(this);

  // Create b2DebugRenderer.
  _b2dr = b2DebugRenderer::create(getWorld());
  _b2dr->retain(); // TODO: remove unique_ptr for _b2dr
  _b2DebugOn = true;
  addChild(_b2dr);

  return true;

  /*
  auto oc = Camera::createOrthographic(winSize.width, winSize.height, 1, 1000);
  oc->setDepth(2);
  oc->setCameraFlag(CameraFlag::USER1);

  const Vec3& eyePosOld = getDefaultCamera()->getPosition3D();
  Vec3 eyePos = {eyePosOld.x, eyePosOld.y, eyePosOld.z};
  oc->setPosition3D(eyePos);
  oc->lookAt(eyePos);
  oc->setPosition(0, 0);
  addChild(oc);


  auto hudLayer = Layer::create();

  auto sprite = Sprite::create("lol.png");
  //sprite->setScaleX(3.0f);
  sprite->getTexture()->setAliasTexParameters();
  //sprite->setPosition(0, 0);

  hudLayer->addChild(sprite);
  hudLayer->setCameraMask((uint16_t) CameraFlag::USER1);
  hudLayer->setPosition(80, winSize.height - sprite->getContentSize().height / 2);
  addChild(hudLayer);
  */
}

void MainGameScene::update(float delta) {
  const float kFps = 60.0f;
  const int kVelocityIterations = 6;
  const int kPositionIterations = 2;
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
