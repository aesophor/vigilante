#include "MainGameScene.h"

#include <string>

#include "SimpleAudioEngine.h"

#include "character/Player.h"
#include "gl/GLESRender.h"
#include "util/box2d/B2DebugRenderer.h"
#include "util/Constants.h"

using std::string;
using std::unique_ptr;
using vigilante::kPPM;
using vigilante::kGravity;
using vigilante::Player;
using vigilante::GameMapManager;
using vigilante::GameInputManager;

USING_NS_CC;

namespace {

// Print useful error message instead of segfaulting when files are not there.
void problemLoading(const char* filename) {
  printf("Error while loading: %s\n", filename);
  printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in MainGameSceneScene.cpp\n");
}

} // namespace

MainGameScene::~MainGameScene() {}


bool MainGameScene::init() {
  if (!Scene::init()) {
    return false;
  }

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();
  log("visible size: %f %f\n", visibleSize.width, visibleSize.height);
  log("origin: %f %f\n", origin.x, origin.y);


  // Initialize GameMapManager
  // b2World is created when GameMapManager's ctor is called.
  _gameMapManager = unique_ptr<GameMapManager>(GameMapManager::getInstance());
  _gameMapManager->setScene(this);
  _gameMapManager->load("Map/starting_point.tmx");
  addChild(_gameMapManager->getMap(), 0);

  // Initialize GameInputManager
  // GameInputManager keep tracks of which keys are pressed.
  _gameInputManager = unique_ptr<GameInputManager>(GameInputManager::getInstance());
  _gameInputManager->activate(this);

  // Create B2DebugRenderer
  auto b2dr = B2DebugRenderer::create(getWorld());
  addChild(b2dr);


  /*
  #define PTM_RATIO 100
  
  b2BodyDef groundBodyDef;
  groundBodyDef.position.Set(0,0);
  b2Body* groundBody = getWorld()->CreateBody(&groundBodyDef);

  b2EdgeShape groundBox;
  b2FixtureDef groundBoxDef;
  groundBoxDef.shape = &groundBox;

  groundBox.Set(b2Vec2(origin.x / PTM_RATIO, origin.y / PTM_RATIO), b2Vec2( (origin.x + visibleSize.width)/PTM_RATIO, origin.y / PTM_RATIO));
  b2Fixture* fixture = groundBody->CreateFixture(&groundBoxDef);

  groundBox.Set(b2Vec2(origin.x / PTM_RATIO, origin.y / PTM_RATIO), b2Vec2(origin.x / PTM_RATIO   , (origin.y + visibleSize.height) / PTM_RATIO));
  groundBody->CreateFixture(&groundBoxDef);

  groundBox.Set(b2Vec2(origin.x  / PTM_RATIO, (origin.y + visibleSize.height) / PTM_RATIO),
      b2Vec2((origin.x + visibleSize.width) / PTM_RATIO, (origin.y + visibleSize.height) / PTM_RATIO));
  groundBody->CreateFixture(&groundBoxDef);

  groundBox.Set(b2Vec2( (origin.x + visibleSize.width) / PTM_RATIO, (origin.y + visibleSize.height) / PTM_RATIO),
      b2Vec2( (origin.x + visibleSize.width) / PTM_RATIO, origin.y / PTM_RATIO));
  groundBody->CreateFixture(&groundBoxDef);

  b2Vec2 center = b2Vec2( (origin.x + visibleSize.width * 0.5) / PTM_RATIO, (origin.y + visibleSize.height * 0.5) / PTM_RATIO);

  for (int i = 0; i < 10; i++) {
    Sprite* ball = Sprite::create("lol.png");
    ball->setTag(1);
    this->addChild(ball);

    b2BodyDef ballBodyDef;
    ballBodyDef.type = b2_dynamicBody;
    ballBodyDef.position.Set(10.0 / PTM_RATIO * ((float)i + 1.0), center.y);
    ballBodyDef.userData = ball;

    b2Body* ballBody = getWorld()->CreateBody(&ballBodyDef);

    b2CircleShape circle;
    circle.m_radius = ball->getContentSize().width * 0.5 / PTM_RATIO;

    b2FixtureDef ballShapeDef;
    ballShapeDef.shape = &circle;
    ballShapeDef.density = 1.0f;
    ballShapeDef.friction = 0.0f;
    ballShapeDef.restitution = .2f;
    b2Fixture* ballFixture = ballBody->CreateFixture(&ballShapeDef);
  }
  */

  schedule(CC_SCHEDULE_SELECTOR(MainGameScene::update));
  


  // Animation
  /*
  SpriteFrameCache* frameCache = SpriteFrameCache::getInstance();
  string location = "Texture/Character/Player/sprites/p_attacking/";
  frameCache->addSpriteFramesWithFile(location + "p_attacking.plist");

  SpriteBatchNode* spritesheet = SpriteBatchNode::create(location + "p_attacking.png");
  addChild(spritesheet);

  Vector<SpriteFrame*> pAttackingFrames;
  for (int i = 0; i <= 7; i++) {
    SpriteFrame* frame = frameCache->getSpriteFrameByName("p_attacking" + std::to_string(i) + ".png");
    pAttackingFrames.pushBack(frame);
  }

  Animation* animation = Animation::createWithSpriteFrames(pAttackingFrames, 0.1);
  Sprite* player = Sprite::createWithSpriteFrameName("p_attacking0.png");

  Size winsize = Director::getInstance()->getWinSize();
  player->setPosition(150, 53);
  player->setScale(2.0f);

  auto sf = Director::getInstance()->getContentScaleFactor();
  Action* action = RepeatForever::create(Animate::create(animation));
  player->runAction(action);
  spritesheet->addChild(player);
  spritesheet->setScale(1 / sf);
  spritesheet->getTexture()->setAliasTexParameters(); // disable texture antialiasing
  */


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
  /*
  for (b2Body* body = getWorld()->GetBodyList(); body; body = body->GetNext()) {
    if (body->GetUserData()) {
      Sprite* sprite = (Sprite*)body->GetUserData();
      Vec2 position = Vec2( body->GetPosition().x * kPPM, body->GetPosition().y * kPPM);
      //log("Position:%.2f,%.2f", position.x, position.y);
      sprite->setPosition(position );
      sprite->setRotation( -1 * CC_RADIANS_TO_DEGREES(body->GetAngle()) );
    }
  }
  */
}

void MainGameScene::handleInput(float delta) {
  auto player = _gameMapManager->getPlayer();

  if (_gameInputManager->isKeyPressed(EventKeyboard::KeyCode::KEY_A)) {
    player->moveLeft();
  } else if (_gameInputManager->isKeyPressed(EventKeyboard::KeyCode::KEY_D)) {
    player->moveRight();
  } else if (_gameInputManager->isKeyJustPressed(EventKeyboard::KeyCode::KEY_W)) {
    player->jump();
  } else if (_gameInputManager->isKeyJustPressed(EventKeyboard::KeyCode::KEY_S)) {

  }
}


b2World* MainGameScene::getWorld() const {
  return _gameMapManager->getWorld();
}
