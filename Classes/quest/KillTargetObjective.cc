// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "KillTargetObjective.h"

using std::string;

namespace vigilante {

KillTargetObjective::KillTargetObjective(const string& desc,
                                         const string& characterName,
                                         int targetAmount)
    : Quest::Objective(Quest::Objective::Type::KILL, desc),
      _characterName(characterName),
      _targetAmount(targetAmount),
      _currentAmount() {}


bool KillTargetObjective::isCompleted() const {
  // TODO: implement this
  return false;
}


const string& KillTargetObjective::getCharacterName() const {
  return _characterName;
}

int KillTargetObjective::getTargetAmount() const {
  return _targetAmount;
}

int KillTargetObjective::getCurrentAmount() const {
  return _currentAmount;
}

void KillTargetObjective::setCurrentAmount(int amount) {
  _currentAmount = amount;
}

} // namespace vigilante
