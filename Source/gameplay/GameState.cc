// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "GameState.h"

#include <vector>

#include "character/Npc.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "util/JsonUtil.h"

using namespace std;

namespace vigilante {

void GameState::save() {
  _json.SetObject();

  _json.AddMember("gameMap", serializeGameMapState(), _allocator);
  _json.AddMember("player", serializePlayerState(), _allocator);
  _json.AddMember("inGameTime", serializeInGameTime(), _allocator);
  _json.AddMember("roomRentalTracker", serializeRoomRentalTrackerState(), _allocator);

  VGLOG(LOG_INFO, "Saving to save file [%s].", _saveFilePath.c_str());
  json_util::saveToFile(_saveFilePath, _json);
}

void GameState::load() {
  VGLOG(LOG_INFO, "Loading from save file [%s].", _saveFilePath.c_str());
  _json = json_util::loadFromFile(_saveFilePath);

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  gmMgr->setNpcsAllowedToAct(false);

  deserializePlayerState(_json["player"].GetObject());
  deserializeGameMapState(_json["gameMap"].GetObject());
  deserializeInGameTime(_json["inGameTime"].GetObject());
  deserializeRoomRentalTrackerState(_json["roomRentalTracker"].GetObject());

  auto hud = SceneManager::the().getCurrentScene<GameScene>()->getHud();
  hud->updateEquippedWeapon();
  hud->updateStatusBars();
}

rapidjson::Value GameState::serializeGameMapState() const {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto gameMap = gmMgr->getGameMap();
  auto player = gmMgr->getPlayer();

  const b2Vec2& playerPos = player->getBody()->GetPosition();
  pair<float, float> playerPosPair{playerPos.x, playerPos.y};

  return json_util::serialize(_allocator,
                              make_pair("tmxTiledMapFilePath", gameMap->getTmxTiledMapFilePath()),
                              make_pair("npcSpawningBlacklist", gmMgr->_npcSpawningBlacklist),
                              make_pair("allPortalStates", gmMgr->_allOpenableObjectStates),
                              make_pair("playerPos", playerPosPair));
}

void GameState::deserializeGameMapState(const rapidjson::Value& obj) const {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto player = gmMgr->getPlayer();

  string tmxTiledMapFilePath;
  pair<float, float> playerPos{0, 0};

  json_util::deserialize(obj,
                         make_pair("tmxTiledMapFilePath", &tmxTiledMapFilePath),
                         make_pair("npcSpawningBlacklist", &gmMgr->_npcSpawningBlacklist),
                         make_pair("allPortalStates", &gmMgr->_allOpenableObjectStates),
                         make_pair("playerPos", &playerPos));

  gmMgr->loadGameMap(tmxTiledMapFilePath, [=](const GameMap*) {
    player->setPosition(playerPos.first, playerPos.second);

    const auto& playerParty = player->getParty();
    for (const auto ally : player->getAllies()) {
      const string& jsonFilePath = ally->getCharacterProfile().jsonFilePath;
      if (auto waitLoc = playerParty->getWaitingMemberLocationInfo(jsonFilePath)) {
        if (waitLoc->tmxMapFilePath == tmxTiledMapFilePath) {
          ally->showOnMap(waitLoc->x * kPpm, waitLoc->y * kPpm);
        }
      } else {
        ally->showOnMap(playerPos.first * kPpm, playerPos.second * kPpm);
      }
    }
  });
}

rapidjson::Value GameState::serializePlayerState() const {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  const auto &profile = gmMgr->getPlayer()->getCharacterProfile();

  return json_util::serialize(_allocator,
                              make_pair("name", profile.name),
                              make_pair("level", profile.level),
                              make_pair("exp", profile.exp),
                              make_pair("fullHealth", profile.fullHealth),
                              make_pair("fullStamina", profile.fullStamina),
                              make_pair("fullMagicka", profile.fullMagicka),
                              make_pair("health", profile.health),
                              make_pair("stamina", profile.stamina),
                              make_pair("magicka", profile.magicka),
                              make_pair("strength", profile.strength),
                              make_pair("dexterity", profile.dexterity),
                              make_pair("intelligence", profile.intelligence),
                              make_pair("luck", profile.luck),
                              make_pair("moveSpeed", profile.moveSpeed),
                              make_pair("jumpHeight", profile.jumpHeight),
                              make_pair("canDoubleJump", profile.canDoubleJump),
                              make_pair("attackForce", profile.attackForce),
                              make_pair("attackTime", profile.attackTime),
                              make_pair("attackRange", profile.attackRange),
                              make_pair("baseMeleeDamage", profile.baseMeleeDamage),
                              make_pair("inventory", serializePlayerInventory()),
                              make_pair("party", serializePlayerParty()));
}

void GameState::deserializePlayerState(const rapidjson::Value& obj) const {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto &profile = gmMgr->getPlayer()->getCharacterProfile();

  rapidjson::Value inventoryJsonObject;
  rapidjson::Value partyJsonObject;

  json_util::deserialize(obj,
                         make_pair("name", &profile.name),
                         make_pair("level", &profile.level),
                         make_pair("exp", &profile.exp),
                         make_pair("fullHealth", &profile.fullHealth),
                         make_pair("fullStamina", &profile.fullStamina),
                         make_pair("fullMagicka", &profile.fullMagicka),
                         make_pair("health", &profile.health),
                         make_pair("stamina", &profile.stamina),
                         make_pair("magicka", &profile.magicka),
                         make_pair("strength", &profile.strength),
                         make_pair("dexterity", &profile.dexterity),
                         make_pair("intelligence", &profile.intelligence),
                         make_pair("luck", &profile.luck),
                         make_pair("moveSpeed", &profile.moveSpeed),
                         make_pair("jumpHeight", &profile.jumpHeight),
                         make_pair("canDoubleJump", &profile.canDoubleJump),
                         make_pair("attackForce", &profile.attackForce),
                         make_pair("attackTime", &profile.attackTime),
                         make_pair("attackRange", &profile.attackRange),
                         make_pair("baseMeleeDamage", &profile.baseMeleeDamage),
                         make_pair("inventory", &inventoryJsonObject),
                         make_pair("party", &partyJsonObject));

  deserializePlayerInventory(inventoryJsonObject);
  deserializePlayerParty(partyJsonObject);
}

rapidjson::Value GameState::serializePlayerInventory() const {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto player = gmMgr->getPlayer();

  map<string, int> itemMapper;
  for (const auto& [itemJsonFilePath, item] : player->_items) {
    itemMapper.insert(std::make_pair(itemJsonFilePath, item->getAmount()));
  }

  vector<vector<string>> inventory(Item::Type::SIZE);
  for (int type = 0; type < Item::Type::SIZE; type++) {
    inventory[type].reserve(player->_inventory[type].size());
    for (const auto item : player->_inventory[type]) {
      inventory[type].push_back(item->getItemProfile().jsonFilePath);
    }
  }

  vector<string> equipmentSlots;
  equipmentSlots.reserve(Equipment::Type::SIZE);
  for (int type = 0; type < Equipment::Type::SIZE; type++) {
    Equipment* equipment = player->_equipmentSlots[type];
    if (equipment) {
      equipmentSlots.push_back(equipment->getItemProfile().jsonFilePath);
    } else {
      equipmentSlots.push_back("");
    }
  }

  return json_util::serialize(_allocator,
                              make_pair("itemMapper", itemMapper),
                              make_pair("inventory", inventory),
                              make_pair("equipmentSlots", equipmentSlots));
}

void GameState::deserializePlayerInventory(const rapidjson::Value& obj) const {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto player = gmMgr->getPlayer();

  map<string, int> itemMapper;
  vector<vector<string>> inventory;
  vector<string> equipmentSlots;

  json_util::deserialize(obj,
                         make_pair("itemMapper", &itemMapper),
                         make_pair("inventory", &inventory),
                         make_pair("equipmentSlots", &equipmentSlots));

  for (int type = 0; type < Equipment::Type::SIZE; type++) {
    player->unequip(static_cast<Equipment::Type>(type), /*audio=*/false);
  }

  player->_items.clear();
  for (const auto& [itemJsonFilePath, amount] : itemMapper) {
    shared_ptr<Item> item = Item::create(itemJsonFilePath);
    item->setAmount(amount);
    player->_items.insert(make_pair(itemJsonFilePath, std::move(item)));
  }

  for (int type = 0; type < Item::Type::SIZE; type++) {
    player->_inventory[type].clear();
    for (const auto& itemJsonFilePath : inventory[type]) {
      auto it = player->_items.find(itemJsonFilePath);
      if (it == player->_items.end()) {
        VGLOG(LOG_ERR, "Failed to find [%s] in player's itemMapper.", itemJsonFilePath.c_str());
        continue;
      }
      player->_inventory[type].insert(it->second.get());
    }
  }

  for (int type = 0; type < Equipment::Type::SIZE; type++) {
    const auto& equipmentJsonFilePath = equipmentSlots[type];
    if (equipmentJsonFilePath == "") {
      player->_equipmentSlots[type] = nullptr;
      continue;
    }

    auto it = player->_items.find(equipmentJsonFilePath);
    if (it == player->_items.end()) {
      VGLOG(LOG_ERR, "Failed to find [%s] in player's itemMapper.", equipmentJsonFilePath.c_str());
      continue;
    }
    auto equipment = dynamic_cast<Equipment*>(it->second.get());
    if (!equipment) {
      VGLOG(LOG_ERR, "[%s] is not an equipment.", equipmentJsonFilePath.c_str());
      continue;
    }
    player->_equipmentSlots[type] = equipment;
  }
}

rapidjson::Value GameState::serializePlayerParty() const {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto player = gmMgr->getPlayer();

  list<string> alliesProfilesFilePaths;
  for (const auto &member : player->getParty()->getMembers()) {
    alliesProfilesFilePaths.push_back(member->getCharacterProfile().jsonFilePath);
  }
  auto alliesJsonObject
    = json_util::makeJsonObject(_allocator, alliesProfilesFilePaths);

  vector<rapidjson::Value> waitingMembersLocationInfos;
  for (const auto& [npcJsonFilePath, locInfo] : player->getParty()->getWaitingMembersLocationInfos()) {
    auto obj = json_util::serialize(_allocator,
                                    make_pair("npcJsonFilePath", npcJsonFilePath),
                                    make_pair("tmxMapFilePath", locInfo.tmxMapFilePath),
                                    make_pair("x", locInfo.x),
                                    make_pair("y", locInfo.y));
    waitingMembersLocationInfos.push_back(std::move(obj));
  }

  return json_util::serialize(_allocator,
                              make_pair("members", std::move(alliesJsonObject)),
                              make_pair("waitingMembersLocationInfo", std::move(waitingMembersLocationInfos)));
}

void GameState::deserializePlayerParty(const rapidjson::Value& obj) const {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto playerParty = gmMgr->getPlayer()->getParty();

  list<string> alliesProfilesFilePaths;
  json_util::deserialize(obj, make_pair("members", &alliesProfilesFilePaths));

  playerParty->dismissAll(/*addToMap=*/false);
  for (const auto& jsonFilePath : alliesProfilesFilePaths) {
    playerParty->addMember(std::make_shared<Npc>(jsonFilePath));
  }

  list<rapidjson::Value> waitingMembersLocationInfo;
  json_util::deserialize(obj,
      make_pair("waitingMembersLocationInfo", &waitingMembersLocationInfo));

  playerParty->_waitingMembersLocationInfos.clear();
  for (const auto& locInfo : waitingMembersLocationInfo) {
    string npcJsonFilePath;
    Party::WaitingLocationInfo info;
    json_util::deserialize(locInfo,
        make_pair("npcJsonFilePath", &npcJsonFilePath),
        make_pair("tmxMapFilePath", &info.tmxMapFilePath),
        make_pair("x", &info.x),
        make_pair("y", &info.y));
    playerParty->_waitingMembersLocationInfos.insert(make_pair(npcJsonFilePath, info));
  }
}

rapidjson::Value GameState::serializeInGameTime() const {
  auto inGameTime = SceneManager::the().getCurrentScene<GameScene>()->getInGameTime();

  return json_util::serialize(_allocator,
                              make_pair("hour", inGameTime->getHour()),
                              make_pair("minute", inGameTime->getMinute()),
                              make_pair("second", inGameTime->getSecond()),
                              make_pair("secondsElapsed", inGameTime->getSecondsElapsed()),
                              make_pair("deferredCmdsMinHeap", inGameTime->getDeferredCmdsMinHeap()));
}

void GameState::deserializeInGameTime(const rapidjson::Value& obj) const {
  int hour{};
  int minute{};
  int second{};
  uint64_t secondsElapsed{};
  vector<InGameTime::DeferredCmd> deferredCmdsMinHeap;

  json_util::deserialize(obj,
                         make_pair("hour", &hour),
                         make_pair("minute", &minute),
                         make_pair("second", &second),
                         make_pair("secondsElapsed", &secondsElapsed),
                         make_pair("deferredCmdsMinHeap", &deferredCmdsMinHeap));

  auto inGameTime = SceneManager::the().getCurrentScene<GameScene>()->getInGameTime();
  inGameTime->setHour(hour);
  inGameTime->setMinute(minute);
  inGameTime->setSecond(second);
  inGameTime->setSecondsElapsed(secondsElapsed);
  inGameTime->setdDeferredCmdsMinHeap(std::move(deferredCmdsMinHeap));
}

rapidjson::Value GameState::serializeRoomRentalTrackerState() const {
  auto roomRentalTracker = SceneManager::the().getCurrentScene<GameScene>()->getRoomRentalTracker();

  return json_util::serialize(_allocator,
                              make_pair("checkedInInns", roomRentalTracker->getCheckedInInns()));
}

void GameState::deserializeRoomRentalTrackerState(const rapidjson::Value& obj) const {
  unordered_set<string> checkedInInns;

  json_util::deserialize(obj, make_pair("checkedInInns", &checkedInInns));

  auto roomRentalTracker = SceneManager::the().getCurrentScene<GameScene>()->getRoomRentalTracker();
  roomRentalTracker->setCheckedInInns(std::move(checkedInInns));
}

}  // namespace vigilante
