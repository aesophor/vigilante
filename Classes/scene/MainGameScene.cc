#include "MainGameScene.h"

#include "SimpleAudioEngine.h"

#include "gl/GLESRender.h"
#include "util/box2d/B2DebugRenderer.h"
#include "util/Constants.h"

using std::unique_ptr;
using vigilante::kPPM;
using vigilante::kGravity;
using vigilante::GameMapManager;

USING_NS_CC;

namespace {

// Print useful error message instead of segfaulting when files are not there.
void problemLoading(const char* filename) {
  printf("Error while loading: %s\n", filename);
  printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in MainGameSceneScene.cpp\n");
}

} // namespace

MainGameScene::~MainGameScene() {}


// on "init" you need to initialize your instance
bool MainGameScene::init() {
  // 1. super init first
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
  _gameMapManager->load("Map/starting_point.tmx");
  addChild(_gameMapManager->getMap(), 0);



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
  
  /*
  // Rectangle shit (using polygon)
  // Create body.
  b2BodyDef bdef;
  bdef.type = b2BodyType::b2_staticBody;
  bdef.position.Set(37.0f / kPPM, 50.0f / kPPM);
  b2Body* body = getWorld()->CreateBody(&bdef);

  // Attach a fixture to body.
  b2PolygonShape shape;
  shape.SetAsBox(33.0f / 2 / kPPM, 74.0f / 2 / kPPM);

  b2FixtureDef fdef;
  fdef.shape = &shape;
  fdef.isSensor = false;
  fdef.friction = 1;
  //fdef.filter.categoryBits = 0;
  body->CreateFixture(&fdef);
  */

  /*
  // Polyline shit
  // Create body.
  b2BodyDef bdef;
  bdef.type = b2BodyType::b2_staticBody;
  bdef.position.SetZero();
  b2Body* body = getWorld()->CreateBody(&bdef);

  b2Vec2 vertices[2] = {{89 / kPPM, 258 / kPPM}, {133 / kPPM, 258 / kPPM}};
  b2ChainShape shape;
  shape.CreateChain(vertices, 2);

  b2FixtureDef fdef;
  fdef.shape = &shape;
  fdef.isSensor = false;
  fdef.friction = 1;
  body->CreateFixture(&fdef);
  */

  this->schedule(CC_SCHEDULE_SELECTOR(MainGameScene::update));
  
  // create debugDrawNode
  auto b = B2DebugRenderer::create(getWorld());
  addChild(b);

//  SpriteBatchNode* spritesheet = SpriteBatchNode::create("Texture/Character/Player/player.png");
//  addChild(spritesheet);

//  auto spriteCache = SpriteFrameCache::getInstance();

  auto camPos = this->getDefaultCamera()->getPosition();
  this->getDefaultCamera()->setPosition(200, 100);
  this->getDefaultCamera()->setPositionZ(225);

  return true;
}

void MainGameScene::update(float delta) {
  int velocityIterations = 5;
  int positionIterations = 1;

  getWorld()->Step(delta, velocityIterations, positionIterations);

  for (b2Body* body = getWorld()->GetBodyList(); body; body = body->GetNext()) {
    if (body->GetUserData()) {
      Sprite* sprite = (Sprite*)body->GetUserData();
      Vec2 position = Vec2( body->GetPosition().x * kPPM, body->GetPosition().y * kPPM);
      //log("Position:%.2f,%.2f", position.x, position.y);
      sprite->setPosition(position );
      sprite->setRotation( -1 * CC_RADIANS_TO_DEGREES(body->GetAngle()) );
    }
  }
}


b2World* MainGameScene::getWorld() const {
  return _gameMapManager->getWorld();
}
