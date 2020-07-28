// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "GameMap.h"

#include "std/make_unique.h"
#include "AssetManager.h"
#include "Constants.h"
#include "character/Character.h"
#include "character/Player.h"
#include "character/Npc.h"
#include "character/Party.h"
#include "item/Equipment.h"
#include "item/Consumable.h"
#include "map/GameMapManager.h"
#include "map/object/Chest.h"
#include "ui/Shade.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/JsonUtil.h"
#include "util/RandUtil.h"

using std::vector;
using std::unordered_set;
using std::string;
using std::unique_ptr;
using cocos2d::Director;
using cocos2d::TMXTiledMap;
using cocos2d::TMXObjectGroup;
using cocos2d::Sequence;
using cocos2d::FadeIn;
using cocos2d::FadeOut;
using cocos2d::CallFunc;

namespace vigilante {

GameMap::GameMap(b2World* world, const string& tmxMapFileName)
    : _world(world),
      _tmxTiledMap(TMXTiledMap::create(tmxMapFileName)),
      _dynamicActors(),
      _portals() {}


void GameMap::createObjects() {
  // Create box2d objects from layers.
  createPolylines("Ground", category_bits::kGround, true, kGroundFriction);
  createPolylines("Wall", category_bits::kWall, true, kWallFriction);
  createRectangles("Platform", category_bits::kPlatform, true, kGroundFriction);
  createPolylines("PivotMarker", category_bits::kPivotMarker, false, 0);
  createPolylines("CliffMarker", category_bits::kCliffMarker, false, 0);
  
  createPortals();
  createChests();
  createNpcs();
}

void GameMap::deleteObjects() {
  // Destroy ground, walls, platforms and portal bodies.
  for (auto body : _tmxTiledMapBodies) {
    _world->DestroyBody(body);
  }

  // Destroy DynamicActor's b2body and textures.
  for (auto& actor : _dynamicActors) {
    actor->removeFromMap();
  }
}

unique_ptr<Player> GameMap::createPlayer() const {
  auto player = std::make_unique<Player>("Resources/Database/character/vlad.json");

  TMXObjectGroup* objGroup = _tmxTiledMap->getObjectGroup("Player");
  auto& valMap = objGroup->getObjects()[0].asValueMap();
  float x = valMap["x"].asFloat();
  float y = valMap["y"].asFloat();

  player->showOnMap(x, y);
  return player;
}

Item* GameMap::spawnItem(const string& itemJson, float x, float y, int amount) {
  Item* item = showDynamicActor<Item>(Item::create(itemJson), x, y);
  item->setAmount(amount);

  float offsetX = rand_util::randFloat(-.3f, .3f);
  float offsetY = 3.0f;
  item->getBody()->ApplyLinearImpulse({offsetX, offsetY}, item->getBody()->GetWorldCenter(), true);

  return item;
}


unordered_set<b2Body*>& GameMap::getTmxTiledMapBodies() {
  return _tmxTiledMapBodies;
}

TMXTiledMap* GameMap::getTmxTiledMap() const {
  return _tmxTiledMap;
}

float GameMap::getWidth() const {
  return _tmxTiledMap->getMapSize().width * _tmxTiledMap->getTileSize().width;
}

float GameMap::getHeight() const {
  return _tmxTiledMap->getMapSize().height * _tmxTiledMap->getTileSize().height;
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
    bool willInteractOnContact = valMap["willInteractOnContact"].asBool();

    b2BodyBuilder bodyBuilder(_world);

    b2Body* body = bodyBuilder.type(b2BodyType::b2_staticBody)
      .position(x + w / 2, y + h / 2, kPpm)
      .buildBody();

    // This portal object will be deleted at GameMap::~GameMap()
    _portals.push_back(std::make_unique<Portal>(targetTmxMapFilePath, targetPortalId,
                                                willInteractOnContact, body));

    bodyBuilder.newRectangleFixture(w / 2, h / 2, kPpm)
      .categoryBits(category_bits::kPortal)
      .setSensor(true)
      .friction(0)
      .setUserData(_portals.back().get())
      .buildFixture();
  }
}

void GameMap::createNpcs() {
  for (auto& rectObj : _tmxTiledMap->getObjectGroup("Npcs")->getObjects()) {
    auto& valMap = rectObj.asValueMap();
    float x = valMap["x"].asFloat();
    float y = valMap["y"].asFloat();
    string json = valMap["json"].asString();

    // Skip this character (don't spawn it)
    // if it has already been recruited by the player.
    // FIXME: what if two or more characters with the same name exist in one map?
    auto player = GameMapManager::getInstance()->getPlayer();
    if (player && player->getParty()->hasMember(json)) {
      continue;
    }

    showDynamicActor(std::make_shared<Npc>(json), x, y);
  }
}

void GameMap::createChests() {
  for (auto& rectObj : _tmxTiledMap->getObjectGroup("Chest")->getObjects()) {
    auto& valMap = rectObj.asValueMap();
    float x = valMap["x"].asFloat();
    float y = valMap["y"].asFloat();
    string items = valMap["items"].asString();
    showDynamicActor(std::make_shared<Chest>(items), x, y);
  }
}


GameMap::Portal::Portal(const string& targetTmxMapFileName, int targetPortalId, bool willInteractOnContact, b2Body* body)
    : _targetTmxMapFileName(targetTmxMapFileName),
      _targetPortalId(targetPortalId),
      _willInteractOnContact(willInteractOnContact),
      _body(body),
      _hintBubbleFxSprite() {}

GameMap::Portal::~Portal() {
  _body->GetWorld()->DestroyBody(_body);
}

void GameMap::Portal::onInteract(Character* user) {
  Shade::getInstance()->getImageView()->runAction(Sequence::create(
    FadeIn::create(Shade::_kFadeInTime),
    CallFunc::create([=]() {
      // Load target GameMap.
      string targetTmxMapFileName = _targetTmxMapFileName;
      int targetPortalId = _targetPortalId;

      auto gmMgr = GameMapManager::getInstance();
      gmMgr->loadGameMap(targetTmxMapFileName);

      auto pos = gmMgr->getGameMap()->_portals.at(targetPortalId)->_body->GetPosition();
      user->setPosition(pos.x, pos.y);

      for (const auto& ally : user->getAllies()) {
        ally->setPosition(pos.x, pos.y);
      }
    }),
    FadeOut::create(Shade::_kFadeOutTime),
    nullptr
  ));
}

void GameMap::Portal::createHintBubbleFx() {
  if (_hintBubbleFxSprite) {
    removeHintBubbleFx();
  }

  const b2Vec2& bodyPos = _body->GetPosition();
  float x = bodyPos.x * kPpm;
  float y = bodyPos.y * kPpm + HINT_BUBBLE_FX_SPRITE_OFFSET_Y;

  _hintBubbleFxSprite
    = GameMapManager::getInstance()->getFxManager()->createFx(
        "Texture/fx/hint_bubble", "dialogue_available", x, y, -1, 45.0f);
}

void GameMap::Portal::removeHintBubbleFx() {
  if (!_hintBubbleFxSprite) {
    return;
  }

  _hintBubbleFxSprite->stopAllActions();
  _hintBubbleFxSprite->removeFromParent();
  _hintBubbleFxSprite = nullptr;
}


const string& GameMap::Portal::getTargetTmxMapFileName() const {
  return _targetTmxMapFileName;
}

int GameMap::Portal::getTargetPortalId() const {
  return _targetPortalId;
}

bool GameMap::Portal::willInteractOnContact() const {
  return _willInteractOnContact;
}

}  // namespace vigilante
