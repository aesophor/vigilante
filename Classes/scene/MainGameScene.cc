#include "MainGameScene.h"

#include <string>

#include "SimpleAudioEngine.h"

#include "AssetManager.h"
#include "Constants.h"
#include "GraphicalLayers.h"
#include "character/Player.h"
#include "input/GameInputManager.h"
#include "map/GameMap.h"
#include "util/box2d/b2DebugRenderer.h"
#include "util/CameraUtil.h"
#include "util/CallbackUtil.h"
#include "util/RandUtil.h"

using std::string;
using std::unique_ptr;
using std::shared_ptr;
using cocos2d::Vec3;
using cocos2d::Camera;
using cocos2d::CameraFlag;
using cocos2d::Director;
using cocos2d::Layer;
using cocos2d::EventKeyboard;
using cocos2d::ui::ImageView;

namespace vigilante {

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
  cocos2d::log("winSize: w=%f h=%f", winSize.width, winSize.height);
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
  
  // Initialize shade.
  _shade = unique_ptr<Shade>(Shade::getInstance());
  _shade->getImageView()->setCameraMask(static_cast<uint16_t>(CameraFlag::USER1));
  addChild(_shade->getImageView(), graphical_layers::kShade);

  // Initialize HUD.
  _hud = unique_ptr<Hud>(Hud::getInstance());
  _hud->getLayer()->setCameraMask(static_cast<uint16_t>(CameraFlag::USER1));
  _hud->getLayer()->setPosition(75, winSize.height - 40);
  addChild(_hud->getLayer(), graphical_layers::kHud);

  // Initialize notification manager.
  _notifications = unique_ptr<NotificationManager>(NotificationManager::getInstance());
  _notifications->getLayer()->setCameraMask(static_cast<uint16_t>(CameraFlag::USER1));
  addChild(_notifications->getLayer(), graphical_layers::kNotification);
  _notifications->show("Notification Manager initialized!");
  _notifications->show("Welcome to Vigilante 0.0.1 alpha");

  // Initialize floating damage manager.
  _floatingDamages = unique_ptr<FloatingDamageManager>(FloatingDamageManager::getInstance());
  addChild(_floatingDamages->getLayer(), graphical_layers::kFloatingDamage);

  // Initialize dialog manager.
  _dialogManager = unique_ptr<DialogManager>(DialogManager::getInstance());
  _dialogManager->getLayer()->setCameraMask(static_cast<uint16_t>(CameraFlag::USER1));
  //_dialogManager->addDialog("???: I'm here to free you");
  //_dialogManager->addDialog("Aesophor: Who... are you");
  //_dialogManager->beginDialog();
  addChild(_dialogManager->getLayer(), graphical_layers::kDialog);

  // Initialize Vigilante's CallbackUtil.
  vigilante::callback_util::init(this);

  // Initialize Vigilante's RandUtil.
  vigilante::rand_util::init();
  
  // Initialize GameMapManager.
  // b2World is created when GameMapManager's ctor is called.
  _gameMapManager = unique_ptr<GameMapManager>(GameMapManager::getInstance());
  _gameMapManager->loadGameMap("Map/prison_cell1.tmx");
  addChild(static_cast<Layer*>(_gameMapManager->getLayer()));

  _gameMapManager->getPlayer()->addItem(new Equipment("Resources/Database/equipment/short_sword.json"), 1);
  _gameMapManager->getPlayer()->addItem(new Equipment("Resources/Database/equipment/royal_cape.json"), 1);

  // Initialize GameInputManager.
  // GameInputManager keep tracks of which keys are pressed.
  GameInputManager::getInstance()->activate(this);

  // Create b2DebugRenderer.
  _b2dr = b2DebugRenderer::create(getWorld());
  _b2dr->setVisible(false);
  addChild(_b2dr);

  _hud->setPlayer(_gameMapManager->getPlayer());

  // Initialize Pause Menu.
  _pauseMenu = unique_ptr<PauseMenu>(new PauseMenu(_gameMapManager->getPlayer()));
  _pauseMenu->getLayer()->setCameraMask(static_cast<uint16_t>(CameraFlag::USER1));
  _pauseMenu->getLayer()->setVisible(false);
  addChild(_pauseMenu->getLayer(), graphical_layers::kPauseMenu);

  // Tick the box2d world.
  schedule(schedule_selector(MainGameScene::update));
  return true;
}

void MainGameScene::update(float delta) {
  // REVIEW this method
  handleInput();

  if (_pauseMenu->getLayer()->isVisible()) {
    return;
  }

  // If there are no ongoing GameMap transitions, then step the box2d world.
  if (_shade->getImageView()->getNumberOfRunningActions() == 0) {
    getWorld()->Step(1 / kFps, kVelocityIterations, kPositionIterations);
  }

  _gameMapManager->update(delta);
  _floatingDamages->update(delta);
  _notifications->update(delta);
  _dialogManager->update(delta);

  vigilante::camera_util::lerpToTarget(_gameCamera, _gameMapManager->getPlayer()->getBody()->GetPosition());
  vigilante::camera_util::boundCamera(_gameCamera, _gameMapManager->getGameMap()->getTmxTiledMap());
  vigilante::camera_util::updateShake(_gameCamera, delta);
}

void MainGameScene::handleInput() {
  auto inputMgr = GameInputManager::getInstance();

  // Toggle b2dr (b2DebugRenderer)
  if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_0)) {
    bool isVisible = _b2dr->isVisible();
    _b2dr->setVisible(!isVisible);
    _notifications->show("[b2dr] is " + std::to_string(!isVisible));
  }

  // Toggle PauseMenu
  if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_ESCAPE)) {
    bool isVisible = _pauseMenu->getLayer()->isVisible();
    _pauseMenu->getLayer()->setVisible(!isVisible);
    _pauseMenu->update();
    return;
  }


  if (_pauseMenu->getLayer()->isVisible()) {
    _pauseMenu->handleInput(); // paused
  } else if (_dialogManager->getLayer()->isVisible()) {
    _dialogManager->handleInput(); // dialog being shown
  } else {
    _gameMapManager->getPlayer()->handleInput(); // not paused
  }
}


b2World* MainGameScene::getWorld() const {
  return _gameMapManager->getWorld();
}

} // namespace vigilante
