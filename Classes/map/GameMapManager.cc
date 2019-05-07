#include "GameMapManager.h"

#include <vector>

#include "Box2D/Box2D.h"

#include "util/CategoryBits.h"
#include "util/Constants.h"

using std::string;
using std::vector;
using cocos2d::TMXTiledMap;
using cocos2d::TMXObjectGroup;
using vigilante::kGround;
using vigilante::kPlayer;
using vigilante::kPPM;
using vigilante::GameMapManager;

USING_NS_CC;

GameMapManager::GameMapManager(const b2Vec2& gravity)
    : _world(new b2World(gravity)), _map() {}

GameMapManager::~GameMapManager() {
  if (_map) {
    _map->release();
  }
  delete _world;
}


void GameMapManager::load(const string& mapFileName) {
  // Release previous TMXTiledMap object.
  _map = TMXTiledMap::create(mapFileName);
 
  // Create box2d objects from layers
  createPolylines(_world, _map, "Ground");
  //createPolylines(_world, _map, "Wall");
  //createRectangles(_world, _map, "Platform");
  //createRectangles(_world, _map, "Portal");

  // Spawn player
  TMXObjectGroup* objGroup = _map->getObjectGroup("Player");
  auto& playerValMap = objGroup->getObjects()[0].asValueMap();
  float playerX = playerValMap["x"].asFloat() / kPPM;
  float playerY = playerValMap["y"].asFloat() / kPPM;
  log("Player pos: %f %f\n", playerX, playerY);

  b2BodyDef bdef;
  bdef.type = b2BodyType::b2_dynamicBody;
  bdef.position.Set(playerX, playerY);
  b2Body* b2body = _world->CreateBody(&bdef);

  // Create body fixture.
  // Fixture position in box2d is relative to b2body's position.
  b2PolygonShape bodyShape;
  b2Vec2 vertices[4];
  vertices[0] = b2Vec2(-5 / kPPM, 20 / kPPM);
  vertices[1] = b2Vec2(5 / kPPM, 20 / kPPM);
  vertices[2] = b2Vec2(-5 / kPPM, -14 / kPPM);
  vertices[3] = b2Vec2(5 / kPPM, -14 / kPPM);
  bodyShape.Set(vertices, 4);

  b2FixtureDef fdef;
  fdef.shape = &bodyShape;
  fdef.filter.categoryBits = kPlayer;
  fdef.filter.maskBits = kGround;
  
  b2Fixture* bodyFixture = b2body->CreateFixture(&fdef);
  //bodyFixture->SetUserData(nullptr); // set to player's *this
}


b2World* GameMapManager::getWorld() const {
  return _world;
}

TMXTiledMap* GameMapManager::getMap() const {
  return _map;
}


void GameMapManager::createRectangles(b2World* world, TMXTiledMap* map, const string& layerName) {
  TMXObjectGroup* portals = map->getObjectGroup(layerName);
  //log("%s\n", _map->getProperty("backgroundMusic").asString().c_str());

  for (auto& obj : portals->getObjects()) {
    //log("%s\n", obj.getDescription().c_str());
    auto& valMap = obj.asValueMap();
    //int id = valMap["id"].asInt();
    float x = valMap["x"].asFloat() / kPPM;
    float y = valMap["y"].asFloat() / kPPM;
    float w = valMap["width"].asFloat() / kPPM;
    float h = valMap["height"].asFloat() / kPPM;
    //cocos2d::log("New collision volume - id: %d, x:%f, y:%f, width:%f, height:%f", id, x, y, w, h);

    b2BodyDef bdef;
    bdef.type = b2BodyType::b2_staticBody;
    bdef.position.Set(x + w / 2, y + h / 2);
    b2Body* body = world->CreateBody(&bdef);

    // Attach a fixture to body.
    b2PolygonShape shape;
    shape.SetAsBox(w / 2, h / 2);

    b2FixtureDef fdef;
    fdef.shape = &shape;
    fdef.isSensor = true;
    fdef.friction = 1;
    //fdef.filter.categoryBits = 0;
    body->CreateFixture(&fdef);
  }
}

void GameMapManager::createPolylines(b2World* world, TMXTiledMap* map, const string& layerName) {
  float scaleFactor = Director::getInstance()->getContentScaleFactor();
  TMXObjectGroup* ground = map->getObjectGroup(layerName);

  for (auto& obj : ground->getObjects()) {
    log("%s\n", obj.getDescription().c_str());
    auto& valMap = obj.asValueMap();
    float xRef = valMap["x"].asFloat() / kPPM;
    float yRef = valMap["y"].asFloat() / kPPM;

    auto& valVec = valMap["polylinePoints"].asValueVector();
    b2Vec2 vertices[valVec.size()];
    for (size_t i = 0; i < valVec.size(); i++) {
      float x = valVec[i].asValueMap()["x"].asFloat() / scaleFactor / kPPM;
      float y = valVec[i].asValueMap()["y"].asFloat() / scaleFactor / kPPM;
      vertices[i] = {xRef + x, yRef - y};
    }

    b2BodyDef bdef;
    bdef.type = b2BodyType::b2_staticBody;
    bdef.position.SetZero();
    b2Body* body = world->CreateBody(&bdef);

    b2ChainShape shape;
    shape.CreateChain(vertices, valVec.size());

    b2FixtureDef fdef;
    fdef.shape = &shape;
    fdef.isSensor = false;
    fdef.friction = 1;
    fdef.filter.categoryBits = kGround;
    body->CreateFixture(&fdef);
  }

}
