#include "GameMap.h"

#include "GameAssetManager.h"
#include "character/Player.h"
#include "character/Enemy.h"
#include "item/Item.h"
#include "item/Equipment.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/CategoryBits.h"
#include "util/Constants.h"

using std::string;
using std::unordered_set;
using cocos2d::Director;
using cocos2d::TMXTiledMap;
using cocos2d::TMXObjectGroup;

namespace vigilante {

GameMap::GameMap(b2World* world, const string& tmxMapFileName)
    : _world(world), _tmxTiledMap(TMXTiledMap::create(tmxMapFileName)) {
  // Create box2d objects from layers
  createPolylines("Ground", category_bits::kGround, true, 2);
  createPolylines("Wall", category_bits::kWall, true, 1);
  createRectangles("Platform", category_bits::kPlatform, true, 2);
  createRectangles("Portal", category_bits::kPortal, false, 0);
  createPolylines("CliffMarker", category_bits::kCliffMarker, false, 0);

  // Spawn an enemy.
  Enemy* enemy = new Enemy("Castle Guard", 300, 100);
  _npcs.insert(enemy);

  // Spawn an item.
  for (int i = 0; i < 10; i++) {
    Item* item = new Equipment(Equipment::Type::WEAPON, "Rusty Axe" + std::to_string(i), "An old rusty axe", asset_manager::kRustyAxeIcon, asset_manager::kRustyAxeSpritesheet, 200, 80);
    _droppedItems.insert(item);
  }
}

GameMap::~GameMap() {
  for (auto body : _tmxTiledMapBodies) {
    _world->DestroyBody(body);
  }

  for (auto npc : _npcs) {
    delete npc;
  }
  for (auto item : _droppedItems) {
    delete item;
  }
}


unordered_set<b2Body*>& GameMap::getTmxTiledMapBodies() {
  return _tmxTiledMapBodies;
}

TMXTiledMap* GameMap::getTmxTiledMap() const {
  return _tmxTiledMap;
}


unordered_set<Character*>& GameMap::getNpcs() {
  return _npcs;
}

unordered_set<Item*>& GameMap::getDroppedItems() {
  return _droppedItems;
}


void GameMap::createRectangles(const string& layerName, short categoryBits, bool collidable, float friction) {
  TMXObjectGroup* portals = _tmxTiledMap->getObjectGroup(layerName);
  //log("%s\n", _map->getProperty("backgroundMusic").asString().c_str());
  
  for (auto& obj : portals->getObjects()) {
    auto& valMap = obj.asValueMap();
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

  for (auto& obj : _tmxTiledMap->getObjectGroup(layerName)->getObjects()) {
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

Player* GameMap::createPlayer() const {
  TMXObjectGroup* objGroup = _tmxTiledMap->getObjectGroup("Player");
  auto& valMap = objGroup->getObjects()[0].asValueMap();
  float x = valMap["x"].asFloat();
  float y = valMap["y"].asFloat();
  return new Player("Aesophor", x, y);
}

} // namespace vigilante
