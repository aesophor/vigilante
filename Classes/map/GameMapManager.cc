#include "GameMapManager.h"

#include <vector>

#include "Box2D/Box2D.h"

#include "util/box2d/b2BodyBuilder.h"
#include "util/CategoryBits.h"
#include "util/Constants.h"

using std::string;
using std::vector;
using cocos2d::TMXTiledMap;
using cocos2d::TMXObjectGroup;

USING_NS_CC;


namespace vigilante {

GameMapManager* GameMapManager::_instance = nullptr;

GameMapManager* GameMapManager::getInstance() {
  if (!_instance) {
    _instance = new GameMapManager({0, kGravity});
  }
  return _instance;
}


GameMapManager::GameMapManager(const b2Vec2& gravity)
    : _world(new b2World(gravity)),
      _worldContactListener(new WorldContactListener()),
      _map(),
      _scene(), 
      _player() {
  _world->SetAllowSleeping(true);
  _world->SetContinuousPhysics(true);
  _world->SetContactListener(_worldContactListener.get());
}

GameMapManager::~GameMapManager() {
  if (_map) {
    _map->release();
  }
  // TODO: clean up b2Bodies here?
}


void GameMapManager::load(const string& mapFileName) {
  // Release previous TMXTiledMap object.
  _map = TMXTiledMap::create(mapFileName);
 
  // Create box2d objects from layers
  createPolylines(_world.get(), _map, "Ground");
  createPolylines(_world.get(), _map, "Wall");
  createRectangles(_world.get(), _map, "Platform");
  createRectangles(_world.get(), _map, "Portal");

  _player = spawnPlayer();
}


Player* GameMapManager::spawnPlayer() {
  TMXObjectGroup* objGroup = _map->getObjectGroup("Player");

  auto& playerValMap = objGroup->getObjects()[0].asValueMap();
  float x = playerValMap["x"].asFloat() / kPPM;
  float y = playerValMap["y"].asFloat() / kPPM;
  log("[INFO] Spawning player at: x=%f y=%f", x, y);

  return new Player(x, y);
}


b2World* GameMapManager::getWorld() const {
  return _world.get();
}

TMXTiledMap* GameMapManager::getMap() const {
  return _map;
}

Player* GameMapManager::getPlayer() const {
  return _player;
}

Scene* GameMapManager::getScene() const {
  return _scene;
}

void GameMapManager::setScene(Scene* scene) {
  _scene = scene;
}


void GameMapManager::createRectangles(b2World* world, TMXTiledMap* map, const string& layerName) {
  TMXObjectGroup* portals = map->getObjectGroup(layerName);
  //log("%s\n", _map->getProperty("backgroundMusic").asString().c_str());
  
  for (auto& obj : portals->getObjects()) {
    auto& valMap = obj.asValueMap();
    float x = valMap["x"].asFloat();
    float y = valMap["y"].asFloat();
    float w = valMap["width"].asFloat();
    float h = valMap["height"].asFloat();

    b2BodyBuilder bodyBuilder(world);

    bodyBuilder.type(b2BodyType::b2_staticBody)
      .position(x + w / 2, y + h / 2, kPPM)
      .buildBody();

    bodyBuilder.newRectangleFixture(w / 2, h / 2, kPPM)
      .categoryBits(kGround)
      .setSensor(true)
      .friction(1)
      .buildFixture();
  }
}

void GameMapManager::createPolylines(b2World* world, TMXTiledMap* map, const string& layerName) {
  float scaleFactor = Director::getInstance()->getContentScaleFactor();

  for (auto& obj : map->getObjectGroup(layerName)->getObjects()) {
    auto& valMap = obj.asValueMap();
    float xRef = valMap["x"].asFloat();
    float yRef = valMap["y"].asFloat();

    auto& valVec = valMap["polylinePoints"].asValueVector();
    b2Vec2 vertices[valVec.size()];
    for (size_t i = 0; i < valVec.size(); i++) {
      float x = valVec[i].asValueMap()["x"].asFloat() / scaleFactor;
      float y = valVec[i].asValueMap()["y"].asFloat() / scaleFactor;
      vertices[i] = {xRef + x, yRef - y};
    }

    b2BodyBuilder bodyBuilder(world);

    bodyBuilder.type(b2BodyType::b2_staticBody)
      .position(0, 0, kPPM)
      .buildBody();

    bodyBuilder.newPolylineFixture(vertices, valVec.size(), kPPM)
      .categoryBits(kGround)
      .friction(1)
      .buildFixture();
  }
}

} // namespace vigilante
