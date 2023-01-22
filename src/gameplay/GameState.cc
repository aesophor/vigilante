// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "GameState.h"

#include <vector>

#include "character/Npc.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "util/JsonUtil.h"

using namespace std;

namespace vigilante {

GameState::GameState(const fs::path& saveFilePath)
    : _saveFilePath(saveFilePath),
      _json(),
      _allocator(_json.GetAllocator()) {}

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
}

rapidjson::Value GameState::serializeGameMapState() const {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto gameMap = gmMgr->getGameMap();
  auto player = gmMgr->getPlayer();

  const b2Vec2& playerPos = player->getBody()->GetPosition();
  pair<float, float> playerPosPair(playerPos.x, playerPos.y);

  return json_util::serialize(_allocator,
                              make_pair("tmxTiledMapFileName", gameMap->getTmxTiledMapFileName()),
                              make_pair("npcSpawningBlacklist", gmMgr->_npcSpawningBlacklist),
                              make_pair("playerPos", playerPosPair));
}

void GameState::deserializeGameMapState(const rapidjson::Value::Object& obj) const {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto player = gmMgr->getPlayer();

  string tmxTiledMapFileName;
  pair<float, float> playerPos(0, 0);

  json_util::deserialize(obj,
                         make_pair("tmxTiledMapFileName", &tmxTiledMapFileName),
                         make_pair("npcSpawningBlacklist", &gmMgr->_npcSpawningBlacklist),
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

  auto partyJsonObject = json_util::serialize(_allocator,
                                              make_pair("members", std::move(alliesJsonObject)),
                                              make_pair("waitingMembersLocationInfo", std::move(waitingMembersLocationInfos)));

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
                              make_pair("party", std::move(partyJsonObject)));
}

void GameState::deserializePlayerState(const rapidjson::Value::Object& obj) const {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto &profile = gmMgr->getPlayer()->getCharacterProfile();
  auto player = gmMgr->getPlayer();

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
                         make_pair("party", &partyJsonObject));

  list<string> alliesProfilesFileNames;
  json_util::deserialize(partyJsonObject,
      make_pair("members", &alliesProfilesFileNames));

  player->getParty()->dismissAll(/*addToMap=*/false);
  for (const auto& jsonFileName : alliesProfilesFileNames) {
    player->getParty()->addMember(std::make_shared<Npc>(jsonFileName));
  }

  list<rapidjson::Value> waitingMembersLocationInfo;
  json_util::deserialize(partyJsonObject,
      make_pair("waitingMembersLocationInfo", &waitingMembersLocationInfo));

  auto &playerPartyWaitingMembersLocationInfo = player->getParty()->_waitingMembersLocationInfos;
  playerPartyWaitingMembersLocationInfo.clear();
  for (const auto& locInfo : waitingMembersLocationInfo) {
    string npcJsonFileName;
    Party::WaitingLocationInfo info;
    json_util::deserialize(locInfo,
                           make_pair("npcJsonFileName", &npcJsonFileName),
                           make_pair("tmxMapFileName", &info.tmxMapFileName),
                           make_pair("x", &info.x),
                           make_pair("y", &info.y));
    playerPartyWaitingMembersLocationInfo.insert(make_pair(npcJsonFileName, info));
  }
}

}  // namespace vigilante
