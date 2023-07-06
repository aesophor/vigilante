// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
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

  VGLOG(LOG_INFO, "Saving to save file [%s].", _saveFilePath.c_str());
  json_util::saveToFile(_saveFilePath, _json);
}

void GameState::load() {
  VGLOG(LOG_INFO, "Loading from save file [%s].", _saveFilePath.c_str());
  _json = json_util::parseJson(_saveFilePath);

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  gmMgr->setNpcsAllowedToAct(false);

  deserializePlayerState(_json["player"].GetObject());
  deserializeGameMapState(_json["gameMap"].GetObject());

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
                              make_pair("tmxTiledMapFileName", gameMap->getTmxTiledMapFileName()),
                              make_pair("npcSpawningBlacklist", gmMgr->_npcSpawningBlacklist),
                              make_pair("allPortalStates", gmMgr->_allOpenableObjectStates),
                              make_pair("playerPos", playerPosPair));
}

void GameState::deserializeGameMapState(const rapidjson::Value& obj) const {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto player = gmMgr->getPlayer();

  string tmxTiledMapFileName;
  pair<float, float> playerPos{0, 0};

  json_util::deserialize(obj,
                         make_pair("tmxTiledMapFileName", &tmxTiledMapFileName),
                         make_pair("npcSpawningBlacklist", &gmMgr->_npcSpawningBlacklist),
                         make_pair("allPortalStates", &gmMgr->_allOpenableObjectStates),
                         make_pair("playerPos", &playerPos));

  gmMgr->loadGameMap(tmxTiledMapFileName, [=]() {
    player->setPosition(playerPos.first, playerPos.second);

    const auto& playerParty = player->getParty();
    for (const auto ally : player->getAllies()) {
      const string& jsonFileName = ally->getCharacterProfile().jsonFileName;
      if (auto waitLoc = playerParty->getWaitingMemberLocationInfo(jsonFileName)) {
        if (waitLoc->tmxMapFileName == tmxTiledMapFileName) {
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
  for (const auto& [itemJsonFileName, item] : player->_itemMapper) {
    itemMapper.insert(std::make_pair(itemJsonFileName, item->getAmount()));
  }

  vector<vector<string>> inventory(Item::Type::SIZE);
  for (int type = 0; type < Item::Type::SIZE; type++) {
    inventory[type].reserve(player->_inventory[type].size());
    for (const auto item : player->_inventory[type]) {
      inventory[type].push_back(item->getItemProfile().jsonFileName);
    }
  }

  vector<string> equipmentSlots;
  equipmentSlots.reserve(Equipment::Type::SIZE);
  for (int type = 0; type < Equipment::Type::SIZE; type++) {
    Equipment* equipment = player->_equipmentSlots[type];
    if (equipment) {
      equipmentSlots.push_back(equipment->getItemProfile().jsonFileName);
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

  player->_itemMapper.clear();
  for (const auto& [itemJsonFileName, amount] : itemMapper) {
    shared_ptr<Item> item = Item::create(itemJsonFileName);
    item->setAmount(amount);
    player->_itemMapper.insert(make_pair(itemJsonFileName, std::move(item)));
  }

  for (int type = 0; type < Item::Type::SIZE; type++) {
    player->_inventory[type].clear();
    for (const auto& itemJsonFileName : inventory[type]) {
      auto it = player->_itemMapper.find(itemJsonFileName);
      if (it == player->_itemMapper.end()) {
        VGLOG(LOG_ERR, "Failed to find [%s] in player's itemMapper.", itemJsonFileName.c_str());
        continue;
      }
      player->_inventory[type].insert(it->second.get());
    }
  }

  for (int type = 0; type < Equipment::Type::SIZE; type++) {
    const auto& equipmentJsonFileName = equipmentSlots[type];
    if (equipmentJsonFileName == "") {
      player->_equipmentSlots[type] = nullptr;
      continue;
    }

    auto it = player->_itemMapper.find(equipmentJsonFileName);
    if (it == player->_itemMapper.end()) {
      VGLOG(LOG_ERR, "Failed to find [%s] in player's itemMapper.", equipmentJsonFileName.c_str());
      continue;
    }
    auto equipment = dynamic_cast<Equipment*>(it->second.get());
    if (!equipment) {
      VGLOG(LOG_ERR, "[%s] is not an equipment.", equipmentJsonFileName.c_str());
      continue;
    }
    player->_equipmentSlots[type] = equipment;
    player->loadEquipmentAnimations(equipment);
  }
}

rapidjson::Value GameState::serializePlayerParty() const {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto player = gmMgr->getPlayer();

  list<string> alliesProfilesFileNames;
  for (const auto &member : player->getParty()->getMembers()) {
    alliesProfilesFileNames.push_back(member->getCharacterProfile().jsonFileName);
  }
  auto alliesJsonObject
    = json_util::makeJsonObject(_allocator, alliesProfilesFileNames);

  vector<rapidjson::Value> waitingMembersLocationInfos;
  for (const auto& [npcJsonFileName, locInfo] : player->getParty()->getWaitingMembersLocationInfos()) {
    auto obj = json_util::serialize(_allocator,
                                    make_pair("npcJsonFileName", npcJsonFileName),
                                    make_pair("tmxMapFileName", locInfo.tmxMapFileName),
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

  list<string> alliesProfilesFileNames;
  json_util::deserialize(obj, make_pair("members", &alliesProfilesFileNames));

  playerParty->dismissAll(/*addToMap=*/false);
  for (const auto& jsonFileName : alliesProfilesFileNames) {
    playerParty->addMember(std::make_shared<Npc>(jsonFileName));
  }

  list<rapidjson::Value> waitingMembersLocationInfo;
  json_util::deserialize(obj,
      make_pair("waitingMembersLocationInfo", &waitingMembersLocationInfo));

  playerParty->_waitingMembersLocationInfos.clear();
  for (const auto& locInfo : waitingMembersLocationInfo) {
    string npcJsonFileName;
    Party::WaitingLocationInfo info;
    json_util::deserialize(locInfo,
        make_pair("npcJsonFileName", &npcJsonFileName),
        make_pair("tmxMapFileName", &info.tmxMapFileName),
        make_pair("x", &info.x),
        make_pair("y", &info.y));
    playerParty->_waitingMembersLocationInfos.insert(make_pair(npcJsonFileName, info));
  }
}

}  // namespace vigilante
