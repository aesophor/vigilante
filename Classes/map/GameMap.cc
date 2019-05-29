#include "GameMap.h"

#include "Constants.h"
#include "CategoryBits.h"
#include "GameAssetManager.h"
#include "character/Player.h"
#include "character/Enemy.h"
#include "character/Npc.h"
#include "item/Item.h"
#include "item/Equipment.h"
#include "map/GameMapManager.h"
#include "ui/Shade.h"
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
  // Create box2d objects from layers.
  createPolylines("Ground", category_bits::kGround, true, 2);
  createPolylines("Wall", category_bits::kWall, true, 1);
  createRectangles("Platform", category_bits::kPlatform, true, 2);
  createPolylines("CliffMarker", category_bits::kCliffMarker, false, 0);
  createPortals();

  // Spawn Npcs and enemies.
  createNpcs();
  createEnemies();
}

GameMap::~GameMap() {
  // Destroy ground, walls, platforms and portal bodies.
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

void GameMap::createNpcs() {
  for (auto& rectObj : _tmxTiledMap->getObjectGroup("Npcs")->getObjects()) {
    auto& valMap = rectObj.asValueMap();
    float x = valMap["x"].asFloat();
    float y = valMap["y"].asFloat();
    string json = valMap["json"].asString();

    Npc* npc = new Npc(json);
    npc->showOnMap(x, y);
    _dynamicActors.insert(npc);
  }
}

void GameMap::createEnemies() {
  for (auto& rectObj : _tmxTiledMap->getObjectGroup("Enemies")->getObjects()) {
    auto& valMap = rectObj.asValueMap();
    float x = valMap["x"].asFloat();
    float y = valMap["y"].asFloat();
    string json = valMap["json"].asString();

    Enemy* enemy = new Enemy(json);
    enemy->showOnMap(x, y);
    _dynamicActors.insert(enemy);
  }
}


GameMap::Portal::Portal(const string& targetTmxMapFileName, int targetPortalId, b2Body* body)
    : _targetTmxMapFileName(targetTmxMapFileName),
      _targetPortalId(targetPortalId),
      _body(body) {}

GameMap::Portal::~Portal() {}

void GameMap::Portal::interact() {
  string targetTmxMapFileName = _targetTmxMapFileName;
  int targetPortalId = _targetPortalId;

  auto gmMgr = GameMapManager::getInstance();
  gmMgr->loadGameMap(targetTmxMapFileName);

  auto pos = gmMgr->getGameMap()->getPortals().at(targetPortalId)->getBody()->GetPosition();
  gmMgr->getPlayer()->setPosition(pos.x, pos.y);
}

const string& GameMap::Portal::getTargetTmxMapFileName() const {
  return _targetTmxMapFileName;
}

int GameMap::Portal::getTargetPortalId() const {
  return _targetPortalId;
}

b2Body* GameMap::Portal::getBody() const {
  return _body;
}

} // namespace vigilante
