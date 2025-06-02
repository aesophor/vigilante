// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "Party.h"

#include <box2d/box2d.h>

#include "Constants.h"
#include "character/Character.h"
#include "character/Npc.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "util/StringUtil.h"

using namespace std;

namespace requiem {

Character* Party::getMember(const string& characterJsonFilePath) const {
  auto it = std::find_if(_members.begin(), _members.end(), [&characterJsonFilePath](const shared_ptr<Character>& c) {
    return c->getCharacterProfile().jsonFilePath == characterJsonFilePath;
  });
  return (it != _members.end()) ? it->get() : nullptr;
}

bool Party::hasMember(const string& characterJsonFilePath) const {
  return getMember(characterJsonFilePath) != nullptr;
}

void Party::recruit(Character* targetCharacter) {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  const b2Vec2 targetPos = targetCharacter->getBody()->GetPosition();

  auto target = gmMgr->getGameMap()->removeDynamicActor<Character>(targetCharacter);
  if (!target) {
    VGLOG(LOG_ERR, "Failed to remove the dynamic actor from game map.");
    return;
  }

  shared_ptr<Npc> targetNpc = std::dynamic_pointer_cast<Npc>(target);
  if (!targetNpc) {
    VGLOG(LOG_ERR, "Failed to recruit the target, because target is not an NPC.");
    return;
  }

  if (!targetNpc->getNpcProfile().isRespawnable) {
    gmMgr->setNpcAllowedToSpawn(targetNpc->getCharacterProfile().jsonFilePath, false);
  }

  target->showOnMap(targetPos.x * kPpm, targetPos.y * kPpm);
  addMember(std::move(target));

  auto notifications = SceneManager::the().getCurrentScene<GameScene>()->getNotifications();
  notifications->show(string_util::format("%s is now following you.",
                                          targetCharacter->getCharacterProfile().name.c_str()));
}

void Party::dismiss(Character* targetCharacter, bool addToMap) {
  b2Body* body = targetCharacter->getBody();
  b2Vec2 targetPos = (body) ? body->GetPosition() : b2Vec2{0, 0};

  removeWaitingMember(targetCharacter->getCharacterProfile().jsonFilePath);

  shared_ptr<DynamicActor> target = removeMember(targetCharacter);
  if (!target) {
    VGLOG(LOG_ERR, "Failed to remove the member from party (member not found).");
    return;
  }

  shared_ptr<Npc> targetNpc = std::dynamic_pointer_cast<Npc>(target);
  if (!targetNpc) {
    VGLOG(LOG_ERR, "Failed to recruit the target, because target is not an NPC.");
    return;
  }

  targetNpc->removeFromMap();

  if (targetNpc->getNpcProfile().isRespawnable) {
    auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
    gmMgr->setNpcAllowedToSpawn(targetNpc->getCharacterProfile().jsonFilePath, true);
  }

  if (addToMap && body) {
    auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
    gmMgr->getGameMap()->showDynamicActor(
        std::move(target), targetPos.x * kPpm, targetPos.y * kPpm);
  }

  auto notifications = SceneManager::the().getCurrentScene<GameScene>()->getNotifications();
  notifications->show(string_util::format("%s has left your party.",
                                          targetCharacter->getCharacterProfile().name.c_str()));
}

void Party::dismissAll(bool addToMap) {
  for (auto it = _members.begin(); it != _members.end();) {
    auto current_it = it;
    ++it;
    dismiss(current_it->get(), addToMap);
  }
}

void Party::askMemberToWait(Character* targetCharacter) {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  const b2Vec2 targetPos = targetCharacter->getBody()->GetPosition();

  addWaitingMember(targetCharacter->getCharacterProfile().jsonFilePath,
                   gmMgr->getGameMap()->getTmxTiledMapFilePath(),
                   targetPos.x,
                   targetPos.y);

  auto notifications = SceneManager::the().getCurrentScene<GameScene>()->getNotifications();
  notifications->show(string_util::format("%s will be waiting for you.",
                                          targetCharacter->getCharacterProfile().name.c_str()));
}

void Party::askMemberToFollow(Character* targetCharacter) {
  removeWaitingMember(targetCharacter->getCharacterProfile().jsonFilePath);

  auto notifications = SceneManager::the().getCurrentScene<GameScene>()->getNotifications();
  notifications->show(string_util::format("%s is now following you.",
                                          targetCharacter->getCharacterProfile().name.c_str()));
}

void Party::addWaitingMember(const string& characterJsonFilePath,
                             const string& currentTmxMapFilePath,
                             float x,
                             float y) {
  auto it = _waitingMembersLocationInfos.find(characterJsonFilePath);
  if (it != _waitingMembersLocationInfos.end()) {
    VGLOG(LOG_ERR, "This member is already a waiting member of the party.");
    return;
  }
  _waitingMembersLocationInfos.insert({characterJsonFilePath, {currentTmxMapFilePath, x, y}});
}

void Party::removeWaitingMember(const string& characterJsonFilePath) {
  if (_waitingMembersLocationInfos.erase(characterJsonFilePath) == 0) {
    VGLOG(LOG_ERR, "This member is not a waiting member of the party.");
  }
}

optional<Party::WaitingLocationInfo>
Party::getWaitingMemberLocationInfo(const std::string& characterJsonFilePath) const {
  auto it = _waitingMembersLocationInfos.find(characterJsonFilePath);
  if (it == _waitingMembersLocationInfos.end()) {
    return std::nullopt;
  }
  return it->second;
}

unordered_set<Character*> Party::getLeaderAndMembers() const {
  unordered_set<Character*> allMembers;

  allMembers.insert(_leader);
  for (const auto& member : _members) {
    allMembers.insert(member.get());
  }
  return allMembers;
}

void Party::dump() {
  VGLOG(LOG_INFO, "Dumping player party");
  for (const auto& member : _members) {
    VGLOG(LOG_INFO, "[%s]", member->getCharacterProfile().jsonFilePath.c_str());
  }
  for (const auto& [npcJsonFilePath, locInfo] : _waitingMembersLocationInfos) {
    VGLOG(LOG_INFO, "[%s] -> {%s, %f, %f}",
          npcJsonFilePath.c_str(), locInfo.tmxMapFilePath.c_str(), locInfo.x, locInfo.y);
  }
}

void Party::addMember(shared_ptr<Character> character) {
  character->setParty(_leader->getParty());
  _members.insert(std::move(character));
}

shared_ptr<Character> Party::removeMember(Character* character) {
  character->setParty(nullptr);

  shared_ptr<Character> removedMember;

  shared_ptr<Character> key(shared_ptr<Character>(), character);
  auto it = _members.find(key);
  if (it == _members.end()) {
    VGLOG(LOG_ERR, "This member is not in the party.");
    return nullptr;
  }

  removedMember = std::move(*it);
  _members.erase(it);
  return removedMember;
}

}  // namespace requiem
