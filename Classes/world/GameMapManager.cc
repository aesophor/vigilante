#include "GameMapManager.h"

#include <vector>

#include "Box2D/Box2D.h"

using std::string;
using std::vector;
using cocos2d::TMXTiledMap;
using cocos2d::TMXObjectGroup;
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

  // _map->getLayer() to get sprite (texture) layers
  // _map->getObjectGroups() to get object layers

  auto scaleFactor = Director::getInstance()->getContentScaleFactor();
  log("scale factor: %f\n", Director::getInstance()->getContentScaleFactor());

  // Extract object info from object groups.
  TMXObjectGroup* portals = _map->getObjectGroup("Portal");
  //log("%s\n", _map->getProperty("backgroundMusic").asString().c_str());

  for (auto& obj : portals->getObjects()) {
    //log("%s\n", obj.getDescription().c_str());
    auto& valMap = obj.asValueMap();
    int id = valMap["id"].asInt();
    float x = valMap["x"].asFloat() / 100;
    float y = valMap["y"].asFloat() / 100;
    float w = valMap["width"].asFloat() / 100;
    float h = valMap["height"].asFloat() / 100;
    //cocos2d::log("New collision volume - id: %d, x:%f, y:%f, width:%f, height:%f", id, x, y, w, h);

    b2BodyDef bdef;
    bdef.type = b2BodyType::b2_staticBody;
    bdef.position.Set(x + w / 2, y + h / 2);
    b2Body* body = getWorld()->CreateBody(&bdef);

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


  TMXObjectGroup* ground = _map->getObjectGroup("Wall");

  for (auto& obj : ground->getObjects()) {
    log("%s\n", obj.getDescription().c_str());
    auto& valMap = obj.asValueMap();
    float xRef = valMap["x"].asFloat() / 100;
    float yRef = valMap["y"].asFloat() / 100;

    auto& valVec = valMap["polylinePoints"].asValueVector();
    b2Vec2 vertices[valVec.size()];
    for (size_t i = 0; i < valVec.size(); i++) {
      float x = valVec[i].asValueMap()["x"].asFloat() / scaleFactor / 100;
      float y = valVec[i].asValueMap()["y"].asFloat() / scaleFactor / 100;
      vertices[i] = {xRef + x, yRef - y};
    }

    b2BodyDef bdef;
    bdef.type = b2BodyType::b2_staticBody;
    bdef.position.SetZero();
    b2Body* body = getWorld()->CreateBody(&bdef);

    b2ChainShape shape;
    shape.CreateChain(vertices, valVec.size());

    b2FixtureDef fdef;
    fdef.shape = &shape;
    fdef.isSensor = false;
    fdef.friction = 1;
    body->CreateFixture(&fdef);
  }
}


b2World* GameMapManager::getWorld() const {
  return _world;
}

TMXTiledMap* GameMapManager::getMap() const {
  return _map;
}
