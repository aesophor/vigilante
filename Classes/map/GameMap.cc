#include "GameMap.h"

#include "Constants.h"
#include "CategoryBits.h"
#include "GameAssetManager.h"
#include "character/Player.h"
#include "character/Enemy.h"
#include "item/Item.h"
#include "item/Equipment.h"
#include "util/box2d/b2BodyBuilder.h"

using std::vector;
using std::unordered_set;
using std::string;
using std::unique_ptr;
using cocos2d::Director;
using cocos2d::TMXTiledMap;
using cocos2d::TMXObjectGroup;

namespace vigilante {

GameMap::GameMap(b2World* world, const string& tmxMapFileName)
    : _world(world),
      _tmxTiledMap(TMXTiledMap::create(tmxMapFileName)),
      _dynamicActors(),
      _portals() {
  // Create box2d objects from layers
  createPolylines("Ground", category_bits::kGround, true, 2);
  createPolylines("Wall", category_bits::kWall, true, 1);
  createRectangles("Platform", category_bits::kPlatform, true, 2);
  createPolylines("CliffMarker", category_bits::kCliffMarker, false, 0);
  createPortals();

  // Spawn an enemy.
  Enemy* enemy = new Enemy("Resources/Database/character/bandit.json");
  enemy->showOnMap(300, 100);
  _dynamicActors.insert(enemy);

  Enemy* enemy1 = new Enemy("Resources/Database/character/skeleton.json");
  enemy1->showOnMap(500, 100);
  _dynamicActors.insert(enemy1);

  Enemy* enemy2 = new Enemy("Resources/Database/character/slime.json");
  enemy2->showOnMap(250, 100);
  _dynamicActors.insert(enemy2);
}

GameMap::~GameMap() {
  for (auto body : _tmxTiledMapBodies) {
    _world->DestroyBody(body);
  }

  for (auto actor : _dynamicActors) {
    actor->removeFromMap();
    delete actor;
  }
  for (auto portal : _portals) {
    delete portal;
  }
}


unordered_set<b2Body*>& GameMap::getTmxTiledMapBodies() {
  return _tmxTiledMapBodies;
}

TMXTiledMap* GameMap::getTmxTiledMap() const {
  return _tmxTiledMap;
}


unordered_set<DynamicActor*>& GameMap::getDynamicActors() {
  return _dynamicActors;
}

const vector<GameMap::Portal*>& GameMap::getPortals() const {
  return _portals;
}


Player* GameMap::createPlayer() const {
  TMXObjectGroup* objGroup = _tmxTiledMap->getObjectGroup("Player");
  auto& valMap = objGroup->getObjects()[0].asValueMap();
  float x = valMap["x"].asFloat();
  float y = valMap["y"].asFloat();
  Player* player = new Player("Resources/Database/character/vlad.json");
  player->showOnMap(x, y);
  return player;
}

void GameMap::createRectangles(const string& layerName, short categoryBits, bool collidable, float friction) {
  TMXObjectGroup* objGroup = _tmxTiledMap->getObjectGroup(layerName);
  //log("%s\n", _map->getProperty("backgroundMusic").asString().c_str());
  
  for (auto& rectObj : objGroup->getObjects()) {
    auto& valMap = rectObj.asValueMap();
    float x = valMap["x"].asFloat();
    float y = valMap["y"].asFloat();
    float w = valMap["width"].asFloat();
    float h = valMap["height"].asFloat();

    b2BodyBuilder bodyBuilder(_world);

    b2Body* body = bodyBuilder.type(b2BodyType::b2_staticBody)
      .position(x + w / 2, y + h / 2, kPpm)
      .buildBody();

    bodyBuilder.newRectangleFixture(w / 2, h / 2, kPpm)
      .categoryBits(categoryBits)
      .setSensor(!collidable)
      .friction(friction)
      .buildFixture();

    _tmxTiledMapBodies.insert(body);
  }
}

void GameMap::createPolylines(const string& layerName, short categoryBits, bool collidable, float friction) {
  float scaleFactor = Director::getInstance()->getContentScaleFactor();

  for (auto& lineObj : _tmxTiledMap->getObjectGroup(layerName)->getObjects()) {
    auto& valMap = lineObj.asValueMap();
    float xRef = valMap["x"].asFloat();
    float yRef = valMap["y"].asFloat();

    auto& valVec = valMap["polylinePoints"].asValueVector();
    b2Vec2 vertices[valVec.size()];
    for (size_t i = 0; i < valVec.size(); i++) {
      float x = valVec[i].asValueMap()["x"].asFloat() / scaleFactor;
      float y = valVec[i].asValueMap()["y"].asFloat() / scaleFactor;
      vertices[i] = {xRef + x, yRef - y};
    }

    b2BodyBuilder bodyBuilder(_world);

    b2Body* body = bodyBuilder.type(b2BodyType::b2_staticBody)
      .position(0, 0, kPpm)
      .buildBody();

    bodyBuilder.newPolylineFixture(vertices, valVec.size(), kPpm)
      .categoryBits(categoryBits)
      .setSensor(!collidable)
      .friction(friction)
      .buildFixture();

    _tmxTiledMapBodies.insert(body);
  }
}

void GameMap::createPortals() {
  for (auto& rectObj : _tmxTiledMap->getObjectGroup("Portal")->getObjects()) {
    auto& valMap = rectObj.asValueMap();
    float x = valMap["x"].asFloat();
    float y = valMap["y"].asFloat();
    float w = valMap["width"].asFloat();
    float h = valMap["height"].asFloat();
    string targetTmxMapFilePath = valMap["targetMap"].asString();
    int targetPortalId = valMap["targetPortalID"].asInt();

    b2BodyBuilder bodyBuilder(_world);

    b2Body* body = bodyBuilder.type(b2BodyType::b2_staticBody)
      .position(x + w / 2, y + h / 2, kPpm)
      .buildBody();

    // This portal object will be deleted at GameMap::~GameMap()
    Portal* portal = new Portal(targetTmxMapFilePath, targetPortalId, body);
    _portals.push_back(portal);

    bodyBuilder.newRectangleFixture(w / 2, h / 2, kPpm)
      .categoryBits(category_bits::kPortal)
      .setSensor(true)
      .friction(0)
      .setUserData(portal)
      .buildFixture();

    _tmxTiledMapBodies.insert(body);
  }
}


GameMap::Portal::Portal(const string& targetTmxMapFileName, int targetPortalId, b2Body* body)
    : targetTmxMapFileName(targetTmxMapFileName),
      targetPortalId(targetPortalId),
      body(body) {}

} // namespace vigilante
