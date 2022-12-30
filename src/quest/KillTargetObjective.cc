// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "KillTargetObjective.h"

using namespace std;

namespace vigilante {

KillTargetObjective::KillTargetObjective(const string& desc,
                                         const string& characterName,
                                         int targetAmount)
    : Quest::Objective(Quest::Objective::Type::KILL, desc),
      _characterName(characterName),
      _targetAmount(targetAmount),
      _currentAmount() {}


bool KillTargetObjective::isCompleted() const {
  return _currentAmount >= _targetAmount;
}

}  // namespace vigilante
