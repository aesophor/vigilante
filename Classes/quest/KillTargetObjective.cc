// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "KillTargetObjective.h"

#include <algorithm>

#include <cocos2d.h>

using std::string;
using std::vector;
using std::unordered_map;

namespace vigilante {

unordered_map<string, vector<KillTargetObjective*>> KillTargetObjective::_relatedObjectives;

KillTargetObjective::KillTargetObjective(const string& desc,
                                         const string& characterName,
                                         int targetAmount)
    : Quest::Objective(Quest::Objective::Type::KILL, desc),
      _characterName(characterName),
      _targetAmount(targetAmount),
      _currentAmount() {}


bool KillTargetObjective::isCompleted() const {
  cocos2d::log("%d/%d", _currentAmount, _targetAmount);
  return _currentAmount >= _targetAmount;
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

void KillTargetObjective::incrementCurrentAmount() {
  _currentAmount++;
}


void KillTargetObjective::addRelatedObjective(const string& characterName, KillTargetObjective* objective) {
  _relatedObjectives[characterName].push_back(objective);
}

void KillTargetObjective::removeRelatedObjective(const string& characterName, KillTargetObjective* objective) {
  auto& objs = _relatedObjectives[characterName];
  objs.erase(std::remove(objs.begin(), objs.end(), objective), objs.end());
}

vector<KillTargetObjective*> KillTargetObjective::getRelatedObjectives(const string& characterName) {
  if (_relatedObjectives.find(characterName) == _relatedObjectives.end()) {
    return {};
  }
  return _relatedObjectives.at(characterName);
}

} // namespace vigilante
