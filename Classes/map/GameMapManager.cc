#include "GameMapManager.h"

#include "Box2D/Box2D.h"

#include "character/Enemy.h"
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
      _layer(Layer::create()),
      _map(),
      _player() {
  _layer->retain();
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
  if (_map) {
    _layer->removeChild(_map);
    _map = nullptr;
  }

  // Release previous TMXTiledMap object.
  _map = TMXTiledMap::create(mapFileName);
  _layer->addChild(_map, 0);
 
  // Create box2d objects from layers
  createPolylines(_world.get(), _map, "Ground", category_bits::kGround, true, 2);
  createPolylines(_world.get(), _map, "Wall", category_bits::kWall, true, 1);
  createRectangles(_world.get(), _map, "Platform", category_bits::kPlatform, true, 2);
  createRectangles(_world.get(), _map, "Portal", category_bits::kPortal, false, 0);

  // Spawn the player.
  _player = spawnPlayer();
  _characters.push_back(_player);
  _layer->addChild(_player->getSpritesheet(), 30);

  // Spawn an enemy.
  Enemy* enemy = new Enemy("Castle Guard", 300, 100);
  _characters.push_back(enemy);
  _layer->addChild(enemy->getSpritesheet(), 31);
}


Player* GameMapManager::spawnPlayer() {
  TMXObjectGroup* objGroup = _map->getObjectGroup("Player");

  auto& playerValMap = objGroup->getObjects()[0].asValueMap();
  float x = playerValMap["x"].asFloat();
  float y = playerValMap["y"].asFloat();
  log("[INFO] Spawning player at: x=%f y=%f", x, y);

  return new Player("Aesophor", x, y);
}


b2World* GameMapManager::getWorld() const {
  return _world.get();
}

Layer* GameMapManager::getLayer() const {
  return _layer;
}

TMXTiledMap* GameMapManager::getMap() const {
  return _map;
}

Player* GameMapManager::getPlayer() const {
  return _player;
}

vector<Character*> GameMapManager::getCharacters() const {
  return _characters;
}


void GameMapManager::createRectangles(b2World* world,
                                      TMXTiledMap* map,
                                      const string& layerName,
                                      short categoryBits,
                                      bool isCollidable,
                                      float friction) {
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
      .position(x + w / 2, y + h / 2, kPpm)
      .buildBody();

    bodyBuilder.newRectangleFixture(w / 2, h / 2, kPpm)
      .categoryBits(categoryBits)
      .setSensor(!isCollidable)
      .friction(friction)
      .buildFixture();
  }
}

void GameMapManager::createPolylines(b2World* world,
                                     TMXTiledMap* map,
                                     const string& layerName,
                                     short categoryBits,
                                     bool isCollidable,
                                     float friction) {
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
      .position(0, 0, kPpm)
      .buildBody();

    bodyBuilder.newPolylineFixture(vertices, valVec.size(), kPpm)
      .categoryBits(categoryBits)
      .setSensor(!isCollidable)
      .friction(friction)
      .buildFixture();
  }
}

} // namespace vigilante
