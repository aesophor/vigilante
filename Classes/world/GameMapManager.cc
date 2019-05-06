#include "GameMapManager.h"

#include <iostream>

using std::cout;
using std::endl;
using std::string;
using cocos2d::TMXTiledMap;
using cocos2d::TMXObjectGroup;
using vigilante::GameMapManager;

GameMapManager::GameMapManager(b2World* world) : _world(world), _map() {}

GameMapManager::~GameMapManager() {
  if (_map) {
    _map->release();
  }
}


void GameMapManager::load(const string& mapFileName) {
  // Release previous TMXTiledMap object.
  _map = TMXTiledMap::create(mapFileName);
  _map->setScale(500 / 100); // temporary

  // _map->getLayer() to get sprite (texture) layers
  // _map->getObjectGroups() to get object layers

  // Extract object info from object groups.
  TMXObjectGroup* ground = _map->getObjectGroup("Ground");
  for (auto& obj : ground->getObjects()) {
    auto& values = obj.asValueMap();
    cout << obj.getDescription() << endl;
    cout << values["x"].asString() << endl;
  }
}

b2World* GameMapManager::getWorld() const {
  return _world;
}

TMXTiledMap* GameMapManager::getMap() const {
  return _map;
}
