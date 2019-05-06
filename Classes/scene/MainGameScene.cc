#include "MainGameScene.h"

#include <iostream>

#include "SimpleAudioEngine.h"

#include "gl/GLESRender.h"
#include "util/box2d/B2DebugDrawLayer.h"
#include "util/Constants.h"

using std::cout;
using std::endl;
using vigilante::kPPM;
using vigilante::kGravity;
using vigilante::GameMapManager;

USING_NS_CC;


// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename) {
  printf("Error while loading: %s\n", filename);
  printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in MainGameSceneScene.cpp\n");
}

MainGameScene::~MainGameScene() { 
  // Release class member objects.
  delete _gameMapManager; // TODO: use unique_ptr
}


// on "init" you need to initialize your instance
bool MainGameScene::init() {
  // 1. super init first
  if (!Scene::init()) {
    return false;
  }

  _gameMapManager = nullptr;

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  /*
  // 2. add a menu item with "X" image, which is clicked to quit the program
  //    you may modify it.

  // add a "close" icon to exit the progress. it's an autorelease object
  auto closeItem = MenuItemImage::create(
      "CloseNormal.png",
      "CloseSelected.png",
      CC_CALLBACK_1(MainGameScene::menuCloseCallback, this));

  if (!closeItem||
      closeItem->getContentSize().width <= 0 ||
      closeItem->getContentSize().height <= 0) {
    problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
  } else {
    float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
    float y = origin.y + closeItem->getContentSize().height/2;
    closeItem->setPosition(Vec2(x,y));
  }

  // create menu, it's an autorelease object
  auto menu = Menu::create(closeItem, NULL);
  menu->setPosition(Vec2::ZERO);
  addChild(menu, 1);
  */

  // 3. add your codes below...
  // add a label shows "Hello World"
  // create and initialize a label

  auto label = Label::createWithTTF("Vigilante", "Font/HeartbitXX.ttf", 24);
  if (!label) {
    problemLoading("'Font/HeartbitXX.ttf'");
  } else {
    // position the label on the center of the screen
    //label->setPosition(Vec2(origin.x + visibleSize.width/2,
    //      origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    //addChild(label, 1);
  }

  // add "MainGameScene" splash screen"
  /*
  auto sprite = Sprite::create("MainGameScene.png");
  if (!sprite) {
    problemLoading("'MainGameScene.png'");
  } else {
    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    addChild(sprite, 0);
  }
  */



  // Create Box2d world by calling GameMapManager's ctor.
  _gameMapManager = new GameMapManager({0, kGravity});
  getWorld()->SetAllowSleeping(true);
  getWorld()->SetContinuousPhysics(true);

  // Load tiled map and add it to our scene.
  _gameMapManager->load("Map/test.tmx");
  //addChild(_gameMapManager->getMap(), 0);
  
  
  /////////////////////////////////////////
  
  
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
  
  // Rectangle shit (using polygon)
  // Create body.
  /*
  b2BodyDef bdef;
  bdef.type = b2BodyType::b2_staticBody;
  bdef.position.Set(583.75 / kPPM, 36.875f / kPPM);
  b2Body* body = world_->CreateBody(&bdef);

  // Attach a fixture to body.
  b2PolygonShape shape;
  shape.SetAsBox(4.375f / 2 / kPPM, 18.125f / 2 / kPPM);

  b2FixtureDef fdef;
  fdef.shape = &shape;
  fdef.isSensor = false;
  fdef.friction = 1;
  //fdef.filter.categoryBits = 0;
  body->CreateFixture(&fdef);
*/

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

  this->schedule(CC_SCHEDULE_SELECTOR(MainGameScene::update));
  /////////////////////////////////////////
  
  // create debugDrawNode
  auto b = B2DebugDrawLayer::create(getWorld());
  addChild(b);

  return true;
}

void MainGameScene::update(float delta) {
  int velocityIterations = 5;
  int positionIterations = 1;

  getWorld()->Step(delta, velocityIterations, positionIterations);

  for (b2Body* body = getWorld()->GetBodyList(); body; body = body->GetNext()) {
    if (body->GetUserData()) {

      Sprite* sprite = (Sprite*)body->GetUserData();
      Vec2 position = Vec2( body->GetPosition().x * PTM_RATIO, body->GetPosition().y * PTM_RATIO) ;
      //log("Position:%.2f,%.2f", position.x, position.y);
      sprite->setPosition(position );
      sprite->setRotation( -1 * CC_RADIANS_TO_DEGREES(body->GetAngle()) );
    }
  }
}


b2World* MainGameScene::getWorld() const {
  return _gameMapManager->getWorld();
}
