#include "GameMapManager.h"

#include <iostream>

using std::cout;
using std::endl;
using std::string;
using cocos2d::TMXTiledMap;
using cocos2d::TMXObjectGroup;
using vigilante::GameMapManager;

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
  //_map->setScale(500 / 100); // temporary

  // _map->getLayer() to get sprite (texture) layers
  // _map->getObjectGroups() to get object layers

  // Extract object info from object groups.
  TMXObjectGroup* ground = _map->getObjectGroup("Portal");

  for (auto& obj : ground->getObjects()) {
    auto& valMap = obj.asValueMap();
    int id = valMap["id"].asInt();
    int x = valMap["real_x"].asInt();
    int y = valMap["real_y"].asInt();
    int w = valMap["real_w"].asInt();
    int h = valMap["real_h"].asInt();
    
    cocos2d::log("New collision volume - id: %d, x:%d, y:%d, width:%d, height:%d", id, x, y, w, h);
  }
}

b2World* GameMapManager::getWorld() const {
  return _world;
}

TMXTiledMap* GameMapManager::getMap() const {
  return _map;
}
