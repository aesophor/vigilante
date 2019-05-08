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
using vigilante::kGravity;
using vigilante::Player;
using vigilante::GameMapManager;

USING_NS_CC;


GameMapManager* GameMapManager::_instance = nullptr;

GameMapManager* GameMapManager::getInstance() {
  if (!_instance) {
    _instance = new GameMapManager({0, kGravity});
  }
  return _instance;
}


GameMapManager::GameMapManager(const b2Vec2& gravity)
    : _world(new b2World(gravity)), _map() {
  _world->SetAllowSleeping(true);
  _world->SetContinuousPhysics(true);
}

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
  createPolylines(_world, _map, "Wall");
  createRectangles(_world, _map, "Platform");
  createRectangles(_world, _map, "Portal");

  Player* player = spawnPlayer();
}


Player* GameMapManager::spawnPlayer() {
  TMXObjectGroup* objGroup = _map->getObjectGroup("Player");

  auto& playerValMap = objGroup->getObjects()[0].asValueMap();
  float x = playerValMap["x"].asFloat() / kPPM;
  float y = playerValMap["y"].asFloat() / kPPM;
  log("[INFO] Spawning player at: x=%f y=%f\n", x, y);

  return new Player(x, y);
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
    auto& valMap = obj.asValueMap();
    float x = valMap["x"].asFloat() / kPPM;
    float y = valMap["y"].asFloat() / kPPM;
    float w = valMap["width"].asFloat() / kPPM;
    float h = valMap["height"].asFloat() / kPPM;

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
