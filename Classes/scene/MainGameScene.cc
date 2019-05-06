#include "MainGameScene.h"

#include <iostream>

#include "SimpleAudioEngine.h"

#include "gl/GLESRender.h"
#include "util/box2d/B2DebugDrawLayer.h"

using std::cout;
using std::endl;

USING_NS_CC;


// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename) {
  printf("Error while loading: %s\n", filename);
  printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in MainGameSceneScene.cpp\n");
}



// on "init" you need to initialize your instance
bool MainGameScene::init() {
  // 1. super init first
  if (!Scene::init()) {
    return false;
  }
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
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
          origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    addChild(label, 1);
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


  map_ = TMXTiledMap::create("Map/village.tmx");
  map_->setScale(500 / 100);

  // Create Box2d world
  const float kGravity = - 32.0f / 0.7f;
  world_ = new b2World({0,0});

  world_->SetAllowSleeping(false);
  //world_->SetContinuousPhysics(true);


  // map_->getLayer() to get sprite (texture) layers
  // map_->getObjectGroups() to get object layers
  TMXObjectGroup* ground = map_->getObjectGroup("Portal");
  
  for (auto& obj : ground->getObjects()) {
    auto& values = obj.asValueMap();
    cout << values.at("x").asString() << endl;
  }
  

  addChild(map_, 0);
  
  
  
  /////////////////////////////////////////
#define PTM_RATIO 100
  b2Vec2 gravity;
  gravity.Set(0.0f, -10.0f);
  world_ = new b2World(gravity);

  b2BodyDef groundBodyDef;
  groundBodyDef.position.Set(0,0);
  b2Body* _groundBody = world_->CreateBody(&groundBodyDef);

  b2EdgeShape groundBox;
  b2FixtureDef groundBoxDef;
  groundBoxDef.shape = &groundBox;

  groundBox.Set(b2Vec2(origin.x / PTM_RATIO, origin.y / PTM_RATIO), b2Vec2( (origin.x + visibleSize.width)/PTM_RATIO, origin.y / PTM_RATIO));
  b2Fixture* fixture = _groundBody->CreateFixture(&groundBoxDef);

  groundBox.Set(b2Vec2(origin.x / PTM_RATIO, origin.y / PTM_RATIO), b2Vec2(origin.x / PTM_RATIO   , (origin.y + visibleSize.height) / PTM_RATIO));
  _groundBody->CreateFixture(&groundBoxDef);

  groundBox.Set(b2Vec2(origin.x  / PTM_RATIO, (origin.y + visibleSize.height) / PTM_RATIO),
      b2Vec2((origin.x + visibleSize.width) / PTM_RATIO, (origin.y + visibleSize.height) / PTM_RATIO));
  _groundBody->CreateFixture(&groundBoxDef);

  groundBox.Set(b2Vec2( (origin.x + visibleSize.width) / PTM_RATIO, (origin.y + visibleSize.height) / PTM_RATIO),
      b2Vec2( (origin.x + visibleSize.width) / PTM_RATIO, origin.y / PTM_RATIO));
  _groundBody->CreateFixture(&groundBoxDef);

  b2Vec2 center = b2Vec2( (origin.x + visibleSize.width * 0.5) / PTM_RATIO, (origin.y + visibleSize.height * 0.5) / PTM_RATIO);

  for (int i = 0; i < 10; i++) {
    Sprite* ball = Sprite::create("lol.png");
    ball->setTag(1);
    this->addChild(ball);

    b2BodyDef ballBodyDef;
    ballBodyDef.type = b2_dynamicBody;
    ballBodyDef.position.Set(10.0 / PTM_RATIO * ((float)i + 1.0), center.y);
    ballBodyDef.userData = ball;

    b2Body* ballBody = world_->CreateBody(&ballBodyDef);

    b2CircleShape circle;
    circle.m_radius = ball->getContentSize().width * 0.5 / PTM_RATIO;

    b2FixtureDef ballShapeDef;
    ballShapeDef.shape = &circle;
    ballShapeDef.density = 1.0f;
    ballShapeDef.friction = 0.0f;
    ballShapeDef.restitution = 1.0f;
    b2Fixture* ballFixture = ballBody->CreateFixture(&ballShapeDef);
  }

  this->schedule(CC_SCHEDULE_SELECTOR(MainGameScene::update));
  /////////////////////////////////////////
  
  // create debugDrawNode
  auto b = B2DebugDrawLayer::create(world_);
  addChild(b);

  return true;
}

void MainGameScene::update(float delta) {
    {
        int velocityIterations = 5;
        int positionIterations = 1;
        
        world_->Step(delta, velocityIterations, positionIterations);
        
        for (b2Body* body = world_->GetBodyList(); body; body = body->GetNext())
        {
            if (body->GetUserData() != NULL) {

                Sprite* sprite = (Sprite*)body->GetUserData();
                Vec2 position = Vec2( body->GetPosition().x * PTM_RATIO, body->GetPosition().y * PTM_RATIO) ;
                CCLOG("Position:%.2f,%.2f", position.x, position.y);
                sprite->setPosition(position );
                sprite->setRotation( -1 * CC_RADIANS_TO_DEGREES(body->GetAngle()) );
            }
        }
    }
}

void MainGameScene::createFixtures(CCTMXLayer* layer) {
  // Create all rectangular fixtures for each tile
  Size layerSize = layer->getLayerSize();
  for (int y = 0; y < layerSize.height; y++) {
    for (int x = 0; x < layerSize.width; x++) {
//      auto tileSprite = layer->getTileAt(Point(x, y));
      // Create a fixture if this tile has a sprite
//      if (tileSprite) {
        //createRectangle(layer, x, y, 1.1f, 1.1f);
//      }
    }
  }
}

void MainGameScene::createRectangle(TMXLayer* layer, int x, int y, float width, float height) {
  auto p = layer->getPositionAt(Point(x, y));
  auto tileSize = map_->getTileSize();
  const float ppm = 32.0f;

  b2BodyDef bodyDef;
  bodyDef.type = b2_staticBody;
  bodyDef.position.Set(
      (p.x + (tileSize.width / 2.0f)) / ppm,
      (p.y + (tileSize.height / 2.0f)) / ppm
  );
  b2Body* body = world_->CreateBody(&bodyDef);

  // Define the shape
  b2PolygonShape shape;
  shape.SetAsBox(
      (tileSize.width / ppm) * 0.5f * width,
      (tileSize.width / ppm) * 0.5f * height
  );

  // Create the fixture
  b2FixtureDef fixtureDef;
  fixtureDef.shape = &shape;
  fixtureDef.density = 1.0f;
  fixtureDef.friction = .3f;
//  fixtureDef.filter.categoryBits = kFilterCategoryLevel;
  fixtureDef.filter.maskBits = 0xffff;
  body->CreateFixture(&fixtureDef);
}


void MainGameScene::menuCloseCallback(Ref* pSender) {
  //Close the cocos2d-x game scene and quit the application
  Director::getInstance()->end();
}
