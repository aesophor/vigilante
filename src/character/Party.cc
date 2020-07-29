// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Party.h"

#include "character/Character.h"

using std::shared_ptr;
using std::string;
using std::unordered_set;

namespace vigilante {

Party::Party(Character* leader) : _leader(leader), _members() {}


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


Character* Party::getLeader() const {
  return _leader;
}

const unordered_set<shared_ptr<Character>>& Party::getMembers() const {
  return _members;
}

unordered_set<Character*> Party::getLeaderAndMembers() const {
  unordered_set<Character*> allMembers;

  allMembers.insert(_leader);
  for (const auto& member : _members) {
    allMembers.insert(member.get());
  }
  return allMembers;
}

}  // namespace vigilante
