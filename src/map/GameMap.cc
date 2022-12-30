// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "GameMap.h"

#include <algorithm>
#include <thread>

#include "Assets.h"
#include "CallbackManager.h"
#include "Constants.h"
#include "character/Character.h"
#include "character/Player.h"
#include "character/Npc.h"
#include "character/Party.h"
#include "item/Equipment.h"
#include "item/Consumable.h"
#include "item/Key.h"
#include "map/FxManager.h"
#include "map/GameMapManager.h"
#include "map/object/Chest.h"
#include "ui/Colorscheme.h"
#include "ui/Shade.h"
#include "ui/console/Console.h"
#include "ui/control_hints/ControlHints.h"
#include "ui/notifications/Notifications.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/StringUtil.h"
#include "util/RandUtil.h"

using namespace std;
USING_NS_CC;

namespace vigilante {

GameMap::GameMap(b2World* world, const string& tmxMapFileName)
    : _world(world),
      _tmxTiledMap(TMXTiledMap::create(tmxMapFileName)),
      _tmxTiledMapFileName(tmxMapFileName),
      _dynamicActors(),
      _triggers(),
      _portals() {}

void GameMap::createObjects() {
  // Create box2d objects from layers.
  createPolylines("Ground", category_bits::kGround, true, kGroundFriction);
  createPolylines("Wall", category_bits::kWall, true, kWallFriction);
  createRectangles("Platform", category_bits::kPlatform, true, kGroundFriction);
  createPolylines("PivotMarker", category_bits::kPivotMarker, false, 0);
  createPolylines("CliffMarker", category_bits::kCliffMarker, false, 0);

  createTriggers();
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
  auto player = std::make_unique<Player>(assets::kPlayerJson);

  const TMXObjectGroup* objGroup = _tmxTiledMap->getObjectGroup("Player");
  const auto& valMap = objGroup->getObjects().at(0).asValueMap();
  float x = valMap.at("x").asFloat();
  float y = valMap.at("y").asFloat();

  player->showOnMap(x, y);
  return player;
}

Item* GameMap::createItem(const string& itemJson, float x, float y, int amount) {
  Item* item = showDynamicActor<Item>(Item::create(itemJson), x, y);
  item->setAmount(amount);

  float offsetX = rand_util::randFloat(-.3f, .3f);
  float offsetY = 3.0f;
  item->getBody()->ApplyLinearImpulse({offsetX, offsetY},
                                      item->getBody()->GetWorldCenter(),
                                      true);
  return item;
}

float GameMap::getWidth() const {
  return _tmxTiledMap->getMapSize().width * _tmxTiledMap->getTileSize().width;
}

float GameMap::getHeight() const {
  return _tmxTiledMap->getMapSize().height * _tmxTiledMap->getTileSize().height;
}

void GameMap::createRectangles(const string& layerName, short categoryBits,
                               bool collidable, float friction) {
  //log("%s\n", _map->getProperty("backgroundMusic").asString().c_str());
  for (const auto& rectObj : _tmxTiledMap->getObjectGroup(layerName)->getObjects()) {
    const auto& valMap = rectObj.asValueMap();
    float x = valMap.at("x").asFloat();
    float y = valMap.at("y").asFloat();
    float w = valMap.at("width").asFloat();
    float h = valMap.at("height").asFloat();

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

void GameMap::createPolylines(const string& layerName, short categoryBits,
                              bool collidable, float friction) {
  float scaleFactor = Director::getInstance()->getContentScaleFactor();

  for (const auto& lineObj : _tmxTiledMap->getObjectGroup(layerName)->getObjects()) {
    const auto& valMap = lineObj.asValueMap();
    float xRef = valMap.at("x").asFloat();
    float yRef = valMap.at("y").asFloat();

    const auto& valVec = valMap.at("polylinePoints").asValueVector();
    b2Vec2 vertices[valVec.size()];
    for (size_t i = 0; i < valVec.size(); i++) {
      float x = valVec.at(i).asValueMap().at("x").asFloat() / scaleFactor;
      float y = valVec.at(i).asValueMap().at("y").asFloat() / scaleFactor;
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

void GameMap::createTriggers() {
  for (const auto& rectObj : _tmxTiledMap->getObjectGroup("Trigger")->getObjects()) {
    const auto& valMap = rectObj.asValueMap();
    float x = valMap.at("x").asFloat();
    float y = valMap.at("y").asFloat();
    float w = valMap.at("width").asFloat();
    float h = valMap.at("height").asFloat();
    vector<string> cmds = string_util::split(valMap.at("cmds").asString(), ';');
    bool canBeTriggeredOnlyOnce = valMap.at("canBeTriggeredOnlyOnce").asBool();
    bool canBeTriggeredOnlyByPlayer = valMap.at("canBeTriggeredOnlyByPlayer").asBool();

    b2BodyBuilder bodyBuilder(_world);

    b2Body* body = bodyBuilder.type(b2BodyType::b2_staticBody)
      .position(x + w / 2, y + h / 2, kPpm)
      .buildBody();

    _triggers.push_back(std::make_unique<GameMap::Trigger>(cmds,
                                                           canBeTriggeredOnlyOnce,
                                                           canBeTriggeredOnlyByPlayer,
                                                           body));

    bodyBuilder.newRectangleFixture(w / 2, h / 2, kPpm)
      .categoryBits(category_bits::kInteractable)
      .setSensor(true)
      .friction(0)
      .setUserData(_triggers.back().get())
      .buildFixture();
  }
}

void GameMap::createPortals() {
  for (const auto& rectObj : _tmxTiledMap->getObjectGroup("Portal")->getObjects()) {
    const auto& valMap = rectObj.asValueMap();
    float x = valMap.at("x").asFloat();
    float y = valMap.at("y").asFloat();
    float w = valMap.at("width").asFloat();
    float h = valMap.at("height").asFloat();
    string targetTmxMapFilePath = valMap.at("targetMap").asString();
    int targetPortalId = valMap.at("targetPortalID").asInt();
    bool willInteractOnContact = valMap.at("willInteractOnContact").asBool();
    bool isLocked = valMap.at("isLocked").asBool();

    b2BodyBuilder bodyBuilder(_world);

    b2Body* body = bodyBuilder.type(b2BodyType::b2_staticBody)
      .position(x + w / 2, y + h / 2, kPpm)
      .buildBody();

    _portals.push_back(std::make_unique<GameMap::Portal>(targetTmxMapFilePath,
                                                         targetPortalId,
                                                         willInteractOnContact,
                                                         isLocked,
                                                         body));

    bodyBuilder.newRectangleFixture(w / 2, h / 2, kPpm)
      .categoryBits(category_bits::kPortal)
      .setSensor(true)
      .friction(0)
      .setUserData(_portals.back().get())
      .buildFixture();
  }
}

void GameMap::createNpcs() {
  for (const auto& rectObj : _tmxTiledMap->getObjectGroup("Npcs")->getObjects()) {
    const auto& valMap = rectObj.asValueMap();
    float x = valMap.at("x").asFloat();
    float y = valMap.at("y").asFloat();
    string json = valMap.at("json").asString();

    if (Npc::isNpcAllowedToSpawn(json)) {
      showDynamicActor(std::make_shared<Npc>(json), x, y);
    }
  }

  auto player = GameMapManager::getInstance()->getPlayer();
  if (!player) {
    return;
  }

  for (const auto& p : player->getParty()->getWaitingMembersLocationInfo()) {
    const string& characterJsonFileName = p.first;
    const Party::WaitingLocationInfo& location = p.second;

    // If this Npc is waiting for its leader in the current map,
    // then we should show it on this map.
    if (location.tmxMapFileName == _tmxTiledMapFileName) {
      player->getParty()->getMember(characterJsonFileName)->showOnMap(location.x * kPpm,
                                                                      location.y * kPpm);
    }
  }
}

void GameMap::createChests() {
  for (const auto& rectObj : _tmxTiledMap->getObjectGroup("Chest")->getObjects()) {
    const auto& valMap = rectObj.asValueMap();
    float x = valMap.at("x").asFloat();
    float y = valMap.at("y").asFloat();
    string items = valMap.at("items").asString();
    showDynamicActor(std::make_shared<Chest>(items), x, y);
  }
}



GameMap::Trigger::Trigger(const vector<string>& cmds,
                          const bool canBeTriggeredOnlyOnce,
                          const bool canBeTriggeredOnlyByPlayer,
                          b2Body* body)
    : _cmds(cmds),
      _canBeTriggeredOnlyOnce(canBeTriggeredOnlyOnce),
      _canBeTriggeredOnlyByPlayer(canBeTriggeredOnlyByPlayer),
      _hasTriggered(),
      _body(body) {}

GameMap::Trigger::~Trigger() {
  _body->GetWorld()->DestroyBody(_body);
}


void GameMap::Trigger::onInteract(Character* user) {
  if (_canBeTriggeredOnlyOnce && _hasTriggered) {
    return;
  }

  if (_canBeTriggeredOnlyByPlayer && !dynamic_cast<Player*>(user)) {
    return;
  }

  _hasTriggered = true;

  for (const auto& cmd : _cmds) {
    Console::getInstance()->executeCmd(cmd);
  }
}

GameMap::Portal::Portal(const string& targetTmxMapFileName, int targetPortalId,
                        bool willInteractOnContact, bool isLocked, b2Body* body)
    : _targetTmxMapFileName(targetTmxMapFileName),
      _targetPortalId(targetPortalId),
      _willInteractOnContact(willInteractOnContact),
      _isLocked(isLocked),
      _body(body),
      _hintBubbleFxSprite() {
  if (GameMap::Portal::hasSavedLockUnlockState(targetTmxMapFileName, targetPortalId)) {
    _isLocked = GameMap::Portal::isLocked(targetTmxMapFileName, targetPortalId);
  }
}

GameMap::Portal::~Portal() {
  _body->GetWorld()->DestroyBody(_body);
}

void GameMap::Portal::onInteract(Character* user) {
  maybeUnlockPortalAs(user);

  if (_isLocked) {
    return;
  }

  string newMapFileName = _targetTmxMapFileName;
  int targetPortalId = _targetPortalId;

  auto afterLoadingGameMap = [user, newMapFileName, targetPortalId]() {
    const b2Vec2& portalPos = GameMapManager::getInstance()
      ->getGameMap()
      ->_portals.at(targetPortalId)
      ->_body->GetPosition();

    // Place the user and its party members at the portal.
    user->setPosition(portalPos.x, portalPos.y);

    // How should we handle user's allies?
    // (1) If `ally` is not waiting for its party leader,
    //     then teleport the ally's body to its party leader.
    // (2) If `ally` is waiting for its party leader,
    //     AND if this new map is not where `ally` is waiting at,
    //     then we'll remove it from the map temporarily.
    //     Whether it will be shown again is determined in
    //     GameMap::spawnNpcs().
    for (auto ally : user->getAllies()) {
      if (!ally->isWaitingForPartyLeader()) {
        ally->setPosition(portalPos.x, portalPos.y);
      } else if (newMapFileName != ally->getParty()->getWaitingMemberLocationInfo(
                 ally->getCharacterProfile().jsonFileName).tmxMapFileName) {
        ally->removeFromMap();
      }
    }
  };

  GameMapManager::getInstance()->loadGameMap(newMapFileName,
                                             afterLoadingGameMap);
}


bool GameMap::Portal::willInteractOnContact() const {
  return _willInteractOnContact;
}

void GameMap::Portal::showHintUI() {
  //createHintBubbleFx();

  string text = "Open";
  Color4B textColor = colorscheme::kWhite;

  if (_isLocked && !canBeUnlockedBy(GameMapManager::getInstance()->getPlayer())) {
    text += " (Locked)";
    textColor = colorscheme::kRed;
  }

  ControlHints::getInstance()->insert({EventKeyboard::KeyCode::KEY_UP_ARROW}, text, textColor);
}

void GameMap::Portal::hideHintUI() {
  //removeHintBubbleFx();
  ControlHints::getInstance()->remove({EventKeyboard::KeyCode::KEY_UP_ARROW});
}

void GameMap::Portal::createHintBubbleFx() {
  if (_hintBubbleFxSprite) {
    removeHintBubbleFx();
  }

  _hintBubbleFxSprite
    = FxManager::getInstance()->createHintBubbleFx(_body, "portal_available");
}

void GameMap::Portal::removeHintBubbleFx() {
  if (!_hintBubbleFxSprite) {
    return;
  }

  FxManager::getInstance()->removeFx(_hintBubbleFxSprite);
  _hintBubbleFxSprite = nullptr;
}

void GameMap::Portal::maybeUnlockPortalAs(Character *user) {
  if (!_isLocked) {
    return;
  }

  if (!canBeUnlockedBy(user)) {
    Notifications::getInstance()->show("This door is locked.");
  } else {
    Notifications::getInstance()->show("Door unlocked.");
    unlock();
  }
}

bool GameMap::Portal::canBeUnlockedBy(Character* user) const {
  const auto& miscItems = user->getInventory()[Item::Type::MISC];
  const auto gameMap = GameMapManager::getInstance()->getGameMap();

  return std::find_if(miscItems.begin(),
                      miscItems.end(),
                      [gameMap, this](const Item* item) {
                          const Key* key = dynamic_cast<const Key*>(item);
                          return key &&
                            key->getKeyProfile().targetTmxFileName == gameMap->_tmxTiledMapFileName &&
                            key->getKeyProfile().targetPortalId == getPortalId();
                      }) != miscItems.end();
}

void GameMap::Portal::lock() {
  _isLocked = true;
  saveLockUnlockState();
}

void GameMap::Portal::unlock() {
  _isLocked = false;
  saveLockUnlockState();
}

bool GameMap::Portal::hasSavedLockUnlockState(const string& tmxMapFileName,
                                              int targetPortalId) {
  auto mapIt = GameMap::Portal::_allPortalStates.find(tmxMapFileName);
  if (mapIt == GameMap::Portal::_allPortalStates.end()) {
    return false;
  }

  return std::find_if(mapIt->second.begin(),
                      mapIt->second.end(),
                      [targetPortalId](const pair<int, bool>& entry) {
                          return entry.first == targetPortalId;
                      }) != mapIt->second.end();
}

bool GameMap::Portal::isLocked(const string& tmxMapFileName,
                               int targetPortalId) {
  auto mapIt = GameMap::Portal::_allPortalStates.find(tmxMapFileName);

  // If we cannot find the associated vector for this tiled map
  // in the unordered_map, then simply return false.
  if (mapIt == GameMap::Portal::_allPortalStates.end()) {
    VGLOG(LOG_WARN, "Unable to find the corresponding portal vector");
    return false;
  }

  // Otherwise, we've found the associated vector of this TiledMap.
  // Now we should find the corresponding entry in that vector
  // and return entry.second which holds the lock state of the Portal.
  for (const auto& entry : mapIt->second) {
    if (entry.first == targetPortalId) {
      return entry.second;
    }
  }

  // If we end up here, then the vector `mapIt->second` doesn't contain
  // the corresponding entry. We should simply return false.
  VGLOG(LOG_WARN, "Unable to find the corresponding entry in the portal vector");
  return false;
}

void GameMap::Portal::setLocked(const string& tmxMapFileName,
                                int targetPortalId,
                                bool locked) {
  auto mapIt = GameMap::Portal::_allPortalStates.find(tmxMapFileName);

  // If we cannot find the associated vector for this tiled map
  // in the unordered_map, then insert a new vector which is
  // initialized with {targetPortalId, locked} and return early.
  if (mapIt == GameMap::Portal::_allPortalStates.end()) {
    GameMap::Portal::_allPortalStates.insert({tmxMapFileName, {{targetPortalId, locked}}});
    return;
  }


  // Otherwise, we've found the associated vector of this TiledMap.
  // Now we should find the corresponding entry in that vector,
  // update that entry, and return early.
  for (auto& entry : mapIt->second) {
    if (entry.first == targetPortalId) {
      entry.second = locked;
      return;
    }
  }

  // If we end up here, then the vector `mapIt->second` doesn't contain
  // the corresponding entry, and thus we have to insert it manually.
  mapIt->second.push_back({targetPortalId, locked});
}

void GameMap::Portal::saveLockUnlockState() const {
  GameMap::Portal::setLocked(_targetTmxMapFileName, _targetPortalId, _isLocked);
}

int GameMap::Portal::getPortalId() const {
  const auto& portals = GameMapManager::getInstance()->getGameMap()->_portals;

  for (size_t i = 0; i < portals.size(); i++) {
    if (portals.at(i).get() == this) {
      return static_cast<int>(i);
    }
  }

  VGLOG(LOG_ERR, "Unable to find the portal ID of: %p", this);
  return -1;
}

}  // namespace vigilante
