#include "GameMapManager.h"

#include "Box2D/Box2D.h"

using std::string;
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

  // Extract object info from object groups.
  TMXObjectGroup* ground = _map->getObjectGroup("Portal");
  //log("%s\n", _map->getProperty("backgroundMusic").asString().c_str());

  for (auto& obj : ground->getObjects()) {
    log("%s\n", obj.getDescription().c_str());
    auto& valMap = obj.asValueMap();
    int id = valMap["id"].asInt();
    float x = valMap["x"].asFloat();
    float y = valMap["y"].asFloat();
    float w = valMap["width"].asFloat();
    float h = valMap["height"].asFloat();
    cocos2d::log("New collision volume - id: %d, x:%f, y:%f, width:%f, height:%f", id, x, y, w, h);

    b2BodyDef bdef;
    bdef.type = b2BodyType::b2_staticBody;
    bdef.position.Set(x / 100 + w / 2 / 100, y / 100 + h / 2 / 100);
    b2Body* body = getWorld()->CreateBody(&bdef);

    // Attach a fixture to body.
    b2PolygonShape shape;
    shape.SetAsBox(w / 2 / 100, h / 2 / 100);

    b2FixtureDef fdef;
    fdef.shape = &shape;
    fdef.isSensor = false;
    fdef.friction = 1;
    //fdef.filter.categoryBits = 0;
    body->CreateFixture(&fdef);
  }





}


b2World* GameMapManager::getWorld() const {
  return _world;
}

TMXTiledMap* GameMapManager::getMap() const {
  return _map;
}
