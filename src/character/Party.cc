// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Party.h"

#include <Box2D/Box2D.h>
#include "Constants.h"
#include "character/Character.h"
#include "map/GameMapManager.h"

using std::shared_ptr;
using std::string;
using std::unordered_set;

namespace vigilante {

Party::Party(Character* leader) : _leader(leader), _members() {}


void Party::recruit(Character* targetCharacter) {
  const b2Vec2 targetPos = targetCharacter->getBody()->GetPosition();

  shared_ptr<Character> target
    = std::dynamic_pointer_cast<Character>(
        GameMapManager::getInstance()->getGameMap()->removeDynamicActor(targetCharacter));

  target->showOnMap(targetPos.x * kPpm, targetPos.y * kPpm);
  addMember(std::move(target));
}

void Party::dismiss(Character* targetCharacter) {
  const b2Vec2 targetPos = targetCharacter->getBody()->GetPosition();

  shared_ptr<DynamicActor> target = removeMember(targetCharacter);
  target->removeFromMap();

  GameMapManager::getInstance()->getGameMap()->showDynamicActor(
      std::move(target), targetPos.x * kPpm, targetPos.y * kPpm);
}

bool Party::hasMember(const string& characterJsonFileName) const {
  for (const auto& member : _members) {
    if (member->getCharacterProfile().jsonFileName == characterJsonFileName) {
      return true;
    }
  }
  return false;
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

const unordered_set<string>& Party::getDeceasedMembers() const {
  return _deceasedMembers;
}

}  // namespace vigilante
