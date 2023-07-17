// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "GameMap.h"

#include <algorithm>
#include <filesystem>
#include <thread>

#include "Assets.h"
#include "Audio.h"
#include "CallbackManager.h"
#include "Constants.h"
#include "character/Character.h"
#include "character/Player.h"
#include "character/Npc.h"
#include "character/Party.h"
#include "item/Equipment.h"
#include "item/Consumable.h"
#include "item/Key.h"
#include "map/object/Chest.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "ui/Colorscheme.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/Logger.h"
#include "util/StringUtil.h"
#include "util/RandUtil.h"

namespace fs = std::filesystem;
using namespace std;
using namespace vigilante::assets;
USING_NS_AX;

namespace vigilante {

GameMap::GameMap(b2World* world, const string& tmxMapFileName)
    : _world{world},
      _tmxTiledMap{TMXTiledMap::create(tmxMapFileName)},
      _tmxTiledMapFileName{tmxMapFileName},
      _bgmFileName{_tmxTiledMap->getProperty("bgm").asString()},
      _parallaxBackground{std::make_unique<ParallaxBackground>()},
      _pathFinder{std::make_unique<SimplePathFinder>()} {}

GameMap::~GameMap() {
  for (auto body : _tmxTiledMapBodies) {
    _world->DestroyBody(body);
  }

  for (auto& actor : _dynamicActors) {
    actor->removeFromMap();
  }
}

void GameMap::update(float delta) {
  _parallaxBackground->update(delta);

  for (auto& actor : _dynamicActors) {
    actor->update(delta);
  }
}

void GameMap::createObjects() {
  list<b2Body*> bodies;

  // Create box2d objects from layers.
  bodies = createPolylines("Ground", category_bits::kGround, true, kGroundFriction);
  _tmxTiledMapBodies.splice(_tmxTiledMapBodies.end(), bodies);

  bodies = createPolylines("Wall", category_bits::kWall, true, kWallFriction);
  _tmxTiledMapBodies.splice(_tmxTiledMapBodies.end(), bodies);

  bodies = createRectangles("Platform", category_bits::kPlatform, true, kGroundFriction);
  _tmxTiledMapPlatformBodies.insert(_tmxTiledMapPlatformBodies.end(), bodies.begin(), bodies.end());
  _tmxTiledMapBodies.splice(_tmxTiledMapBodies.end(), bodies);

  bodies = createPolylines("PivotMarker", category_bits::kPivotMarker, false, 0);
  _tmxTiledMapBodies.splice(_tmxTiledMapBodies.end(), bodies);

  bodies = createPolylines("CliffMarker", category_bits::kCliffMarker, false, 0);
  _tmxTiledMapBodies.splice(_tmxTiledMapBodies.end(), bodies);

  createTriggers();
  createPortals();
  createChests();
  createNpcs();
  createParallaxBackground();
}

unique_ptr<Player> GameMap::createPlayer() const {
  auto player = std::make_unique<Player>(assets::kPlayerJson);

  const TMXObjectGroup* objGroup = _tmxTiledMap->getObjectGroup("Player");
  const auto& valMap = objGroup->getObjects().at(0).asValueMap();
  float x = valMap.at("x").asFloat();
  float y = valMap.at("y").asFloat();

  if (auto it = valMap.find("isFacingRight"); it != valMap.end()) {
    player->setFacingRight(it->second.asBool());
  }

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

ax::ValueVector GameMap::getObjects(const string& layerName) {
  auto objectGroup = _tmxTiledMap->getObjectGroup(layerName);
  if (!objectGroup) {
    VGLOG(LOG_INFO, "Failed to get object object group: [%s]", layerName.c_str());
    return {};
  }

  return objectGroup->getObjects();
}

list<b2Body*> GameMap::createRectangles(const string& layerName, short categoryBits,
                                             bool collidable, float friction) {
  list<b2Body*> bodies;

  for (const auto& rectObj : getObjects(layerName)) {
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

    bodies.emplace_back(body);
  }

  return bodies;
}

list<b2Body*> GameMap::createPolylines(const string& layerName, short categoryBits,
                                            bool collidable, float friction) {
  list<b2Body*> bodies;
  float scaleFactor = Director::getInstance()->getContentScaleFactor();

  for (const auto& lineObj : getObjects(layerName)) {
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

    bodies.emplace_back(body);
  }

  return bodies;
}

void GameMap::createTriggers() {
  for (const auto& rectObj : getObjects("Trigger")) {
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
  for (const auto& rectObj : getObjects("Portal")) {
    const auto& valMap = rectObj.asValueMap();
    float x = valMap.at("x").asFloat();
    float y = valMap.at("y").asFloat();
    float w = valMap.at("width").asFloat();
    float h = valMap.at("height").asFloat();
    string destTmxMapFilePath = valMap.at("destMap").asString();
    int destPortalId = valMap.at("destPortalID").asInt();
    bool willInteractOnContact = valMap.at("willInteractOnContact").asBool();
    bool isLocked = valMap.at("isLocked").asBool();

    b2BodyBuilder bodyBuilder(_world);

    b2Body* body = bodyBuilder.type(b2BodyType::b2_staticBody)
      .position(x + w / 2, y + h / 2, kPpm)
      .buildBody();

    _portals.push_back(std::make_unique<GameMap::Portal>(destTmxMapFilePath,
                                                         destPortalId,
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
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();

  for (const auto& rectObj : getObjects("Npcs")) {
    const auto& valMap = rectObj.asValueMap();
    float x = valMap.at("x").asFloat();
    float y = valMap.at("y").asFloat();
    string json = valMap.at("json").asString();

    if (!gmMgr->isNpcAllowedToSpawn(json)) {
      continue;
    }

    auto npc = std::make_shared<Npc>(json);
    if (auto it = valMap.find("isFacingRight"); it != valMap.end()) {
      npc->setFacingRight(it->second.asBool());
    }
    showDynamicActor(std::move(npc), x, y);
  }

  auto player = gmMgr->getPlayer();
  if (!player) {
    return;
  }

  for (const auto& p : player->getParty()->getWaitingMembersLocationInfos()) {
    const string& characterJsonFileName = p.first;
    const auto& waitLoc = p.second;

    if (waitLoc.tmxMapFileName == _tmxTiledMapFileName) {
      auto member = player->getParty()->getMember(characterJsonFileName);
      if (!member) {
        VGLOG(LOG_ERR, "Character [%s] is not in the player's party.", characterJsonFileName.c_str());
        continue;
      }
      member->showOnMap(waitLoc.x * kPpm, waitLoc.y * kPpm);
    }
  }
}

void GameMap::createChests() {
  ax::ValueVector objects = getObjects("Chest");
  for (int i = 0; i < objects.size(); i++) {
    const auto& valMap = objects[i].asValueMap();
    float x = valMap.at("x").asFloat();
    float y = valMap.at("y").asFloat();
    string items = valMap.at("items").asString();

    auto chest = std::make_shared<Chest>(_tmxTiledMapFileName, i, items);
    showDynamicActor(std::move(chest), x, y);
  }
}

void GameMap::createParallaxBackground() {
  const Value property = _tmxTiledMap->getProperty("parallaxBackground");
  if (property.isNull()) {
    VGLOG(LOG_INFO, "Failed to find parallaxBackground property from tmx, skip creating parallax bg.");
    return;
  }
  
  const fs::path bgDirPath{property.asString()};
  if (!_parallaxBackground->load(bgDirPath)) {
    VGLOG(LOG_ERR, "Failed to load parallax background from dir: [%s].", bgDirPath.c_str());
    return;
  }
  
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  gmMgr->getLayer()->addChild(_parallaxBackground->getParallaxNode(), graphical_layers::kParallaxBackground);
}

GameMap::Trigger::Trigger(const vector<string>& cmds,
                          const bool canBeTriggeredOnlyOnce,
                          const bool canBeTriggeredOnlyByPlayer,
                          b2Body* body)
    : _cmds{cmds},
      _canBeTriggeredOnlyOnce{canBeTriggeredOnlyOnce},
      _canBeTriggeredOnlyByPlayer{canBeTriggeredOnlyByPlayer},
      _body{body} {}

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

  auto console = SceneManager::the().getCurrentScene<GameScene>()->getConsole();
  for (const auto& cmd : _cmds) {
    console->executeCmd(cmd);
  }
}

GameMap::Portal::Portal(const string& destTmxMapFileName, int destPortalId,
                        bool willInteractOnContact, bool isLocked, b2Body* body)
    : _destTmxMapFileName{destTmxMapFileName},
      _destPortalId{destPortalId},
      _willInteractOnContact{willInteractOnContact},
      _isLocked{isLocked},
      _body{body} {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  constexpr auto kType = OpenableObjectType::PORTAL;
  if (gmMgr->hasSavedOpenedClosedState(destTmxMapFileName, kType, destPortalId)) {
    _isLocked = !gmMgr->isOpened(destTmxMapFileName, kType, destPortalId);
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

  string destMapFileName = _destTmxMapFileName;
  int destPortalId = _destPortalId;

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto afterLoadingGameMap = [user, destMapFileName, destPortalId, gmMgr]() {
    const auto& portals = gmMgr->getGameMap()->_portals;
    assert(destPortalId < portals.size());

    const auto& portalPos = portals[destPortalId]->_body->GetPosition();
    user->setPosition(portalPos.x, portalPos.y);

    for (auto ally : user->getAllies()) {
      if (!ally->isWaitingForPartyLeader()) {
        ally->setPosition(portalPos.x, portalPos.y);
      } else if (destMapFileName != ally->getParty()->getWaitingMemberLocationInfo(
                 ally->getCharacterProfile().jsonFileName)->tmxMapFileName) {
        ally->removeFromMap();
      }
    }
  };

  gmMgr->loadGameMap(destMapFileName, afterLoadingGameMap);
}

bool GameMap::Portal::willInteractOnContact() const {
  return _willInteractOnContact;
}

void GameMap::Portal::showHintUI() {
  //createHintBubbleFx();

  string text = "Open";
  Color4B textColor = colorscheme::kWhite;

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  if (_isLocked && !canBeUnlockedBy(gmMgr->getPlayer())) {
    text += " (Locked)";
    textColor = colorscheme::kRed;
  }

  auto controlHints = SceneManager::the().getCurrentScene<GameScene>()->getControlHints();
  controlHints->insert({EventKeyboard::KeyCode::KEY_UP_ARROW}, text, textColor);
}

void GameMap::Portal::hideHintUI() {
  //removeHintBubbleFx();

  auto controlHints = SceneManager::the().getCurrentScene<GameScene>()->getControlHints();
  controlHints->remove({EventKeyboard::KeyCode::KEY_UP_ARROW});
}

void GameMap::Portal::createHintBubbleFx() {
  if (_hintBubbleFxSprite) {
    removeHintBubbleFx();
  }

  auto fxMgr = SceneManager::the().getCurrentScene<GameScene>()->getFxManager();
  _hintBubbleFxSprite = fxMgr->createHintBubbleFx(_body, "portal_available");
}

void GameMap::Portal::removeHintBubbleFx() {
  if (!_hintBubbleFxSprite) {
    return;
  }

  auto fxMgr = SceneManager::the().getCurrentScene<GameScene>()->getFxManager();
  fxMgr->removeFx(_hintBubbleFxSprite);
  _hintBubbleFxSprite = nullptr;
}

void GameMap::Portal::maybeUnlockPortalAs(Character *user) {
  if (!_isLocked) {
    return;
  }

  auto notifications = SceneManager::the().getCurrentScene<GameScene>()->getNotifications();
  if (!canBeUnlockedBy(user)) {
    notifications->show("This door is locked.");
    Audio::the().playSfx(kSfxDoorLocked);
  } else {
    notifications->show("Door unlocked.");
    Audio::the().playSfx(kSfxDoorUnlocked);
    unlock();
  }
}

bool GameMap::Portal::canBeUnlockedBy(Character* user) const {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto gameMap = gmMgr->getGameMap();
  const auto& miscItems = user->getInventory()[Item::Type::MISC];

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

void GameMap::Portal::saveLockUnlockState() const {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  constexpr auto kType = OpenableObjectType::PORTAL;
  gmMgr->setOpened(_destTmxMapFileName, kType, _destPortalId, !_isLocked);
}

int GameMap::Portal::getPortalId() const {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  const auto& portals = gmMgr->getGameMap()->_portals;

  for (size_t i = 0; i < portals.size(); i++) {
    if (portals.at(i).get() == this) {
      return static_cast<int>(i);
    }
  }

  VGLOG(LOG_ERR, "Unable to find the portal ID of: %p", this);
  return -1;
}

}  // namespace vigilante
