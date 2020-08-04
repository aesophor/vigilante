// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Party.h"

#include <Box2D/Box2D.h>
#include "Constants.h"
#include "character/Character.h"
#include "map/GameMapManager.h"
#include "ui/notifications/Notifications.h"
#include "util/StringUtil.h"

using std::string;
using std::shared_ptr;
using std::unordered_map;
using std::unordered_set;

namespace vigilante {

Party::Party(Character* leader)
    : _leader(leader),
      _members(),
      _waitingMembersLocationInfo(),
      _deceasedMembers() {}


Character* Party::getMember(const string& characterJsonFileName) const {
  auto it = std::find_if(_members.begin(), _members.end(), [&](const shared_ptr<Character>& c) {
      return c->getCharacterProfile().jsonFileName == characterJsonFileName;
  });
  return (it != _members.end()) ? it->get() : nullptr;
}

bool Party::hasMember(const string& characterJsonFileName) const {
  return getMember(characterJsonFileName) != nullptr;
}

void Party::recruit(Character* targetCharacter) {
  const b2Vec2 targetPos = targetCharacter->getBody()->GetPosition();

  shared_ptr<Character> target
    = std::dynamic_pointer_cast<Character>(
        GameMapManager::getInstance()->getGameMap()->removeDynamicActor(targetCharacter));

  target->showOnMap(targetPos.x * kPpm, targetPos.y * kPpm);
  addMember(std::move(target));

  Notifications::getInstance()->show(
      string_util::format("%s is now following you.", targetCharacter->getCharacterProfile().name.c_str()));
}

void Party::dismiss(Character* targetCharacter, bool addToMap) {
  if (hasWaitingMember(targetCharacter->getCharacterProfile().jsonFileName)) {
    removeWaitingMember(targetCharacter->getCharacterProfile().jsonFileName);
  }

  const b2Vec2 targetPos = targetCharacter->getBody()->GetPosition();
  
  shared_ptr<DynamicActor> target = removeMember(targetCharacter);
  target->removeFromMap();

  if (addToMap) {
    GameMapManager::getInstance()->getGameMap()->showDynamicActor(
        std::move(target), targetPos.x * kPpm, targetPos.y * kPpm);
  }

  Notifications::getInstance()->show(
      string_util::format("%s has left your party.", targetCharacter->getCharacterProfile().name.c_str()));
}


void Party::askMemberToWait(Character* targetCharacter) {
  const b2Vec2 targetPos = targetCharacter->getBody()->GetPosition();

  addWaitingMember(targetCharacter->getCharacterProfile().jsonFileName,
                   GameMapManager::getInstance()->getGameMap()->getTmxTiledMapFileName(),
                   targetPos.x,
                   targetPos.y);

  Notifications::getInstance()->show(
      string_util::format("%s will be waiting for you.", targetCharacter->getCharacterProfile().name.c_str()));
}

void Party::askMemberToFollow(Character* targetCharacter) {
  removeWaitingMember(targetCharacter->getCharacterProfile().jsonFileName);

  Notifications::getInstance()->show(
      string_util::format("%s is now following you.", targetCharacter->getCharacterProfile().name.c_str()));
}


bool Party::hasWaitingMember(const string& characterJsonFileName) const {
  auto it = _waitingMembersLocationInfo.find(characterJsonFileName);
  return it != _waitingMembersLocationInfo.end();
}

void Party::addWaitingMember(const string& characterJsonFileName,
                             const string& currentTmxMapFileName,
                             float x,
                             float y) {
  if (hasWaitingMember(characterJsonFileName)) {
    VGLOG(LOG_ERR, "This member is already a waiting member of the party.");
    return;
  }
  _waitingMembersLocationInfo.insert({characterJsonFileName, {currentTmxMapFileName, x, y}});
}

void Party::removeWaitingMember(const string& characterJsonFileName) {
  if (!hasWaitingMember(characterJsonFileName)) {
    VGLOG(LOG_ERR, "This member is not a waiting member of the party.");
    return;
  }
  _waitingMembersLocationInfo.erase(characterJsonFileName);
}

Party::WaitingLocationInfo
Party::getWaitingMemberLocationInfo(const std::string& characterJsonFileName) const {
  auto it = _waitingMembersLocationInfo.find(characterJsonFileName);
  if (it == _waitingMembersLocationInfo.end()) {
    VGLOG(LOG_ERR, "This member is not a waiting member of the party");
    return {"", 0, 0};
  }
  return it->second;
}


bool Party::hasDeceasedMember(const string& characterJsonFileName) const {
  auto it = _deceasedMembers.find(characterJsonFileName);
  return it != _deceasedMembers.end();
}

void Party::addDeceasedMember(const string& characterJsonFileName) {
  if (hasDeceasedMember(characterJsonFileName)) {
    VGLOG(LOG_ERR, "This member is already a deceased member of the party.");
    return;
  }
  _deceasedMembers.insert(characterJsonFileName);
}

void Party::removeDeceasedMember(const string& characterJsonFileName) {
  if (!hasDeceasedMember(characterJsonFileName)) {
    VGLOG(LOG_ERR, "This member is not a deceased member of the party.");
    return;
  }
  _deceasedMembers.erase(characterJsonFileName);
}


Character* Party::getLeader() const {
  return _leader;
}

unordered_set<Character*> Party::getLeaderAndMembers() const {
  unordered_set<Character*> allMembers;

  allMembers.insert(_leader);
  for (const auto& member : _members) {
    allMembers.insert(member.get());
  }
  return allMembers;
}

const unordered_set<shared_ptr<Character>>& Party::getMembers() const {
  return _members;
}

const unordered_map<string, Party::WaitingLocationInfo>&
Party::getWaitingMembersLocationInfo() const {
  return _waitingMembersLocationInfo;
}

const unordered_set<string>& Party::getDeceasedMembers() const {
  return _deceasedMembers;
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

}  // namespace vigilante
