// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "KillTargetObjective.h"

using std::string;

namespace vigilante {

KillTargetObjective::KillTargetObjective(const string& desc,
                                         const string& characterName,
                                         int amount)
    : Quest::Objective(Quest::Objective::Type::KILL, desc),
      _characterName(characterName),
      _amount(amount) {}


bool KillTargetObjective::isCompleted() const {
  // TODO: implement this
  return false;
}


const string& KillTargetObjective::getCharacterName() const {
  return _characterName;
}

int KillTargetObjective::getAmount() const {
  return _amount;
}

} // namespace vigilante
