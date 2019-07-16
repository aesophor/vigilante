// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "KillTargetObjective.h"

using std::string;

namespace vigilante {

KillTargetObjective::KillTargetObjective(Quest* quest, const string& desc, Character* target)
    : Quest::Objective(quest, Quest::Objective::Type::KILL, desc),
      _target(target) {}


bool KillTargetObjective::isCompleted() const {
  return _target->isKilled();
}

Character* KillTargetObjective::getTarget() const {
  return _target;
}

} // namespace vigilante
