// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Quest.h"

#include <algorithm>

#include <json/document.h>
#include "quest/CollectItemObjective.h"
#include "quest/KillTargetObjective.h"
#include "util/JsonUtil.h"

using std::string;
using std::vector;
using std::unordered_map;
using rapidjson::Document;

namespace vigilante {

Quest::Quest(const string& jsonFileName)
    : _questProfile(jsonFileName),
      _isUnlocked(),
      _currentStageIdx(-1) {}

Quest::~Quest() {
  for (const auto& stage : _questProfile.stages) {
    delete stage.objective;
  }
}


void Quest::import(const string& jsonFileName) {
  _questProfile = Quest::Profile(jsonFileName);
}

void Quest::unlock() {
  _isUnlocked = true;
}

void Quest::advanceStage() {
  if (isCompleted()) {
    return;
  }
 
  if (_currentStageIdx >= 0) {
    switch (getCurrentStage().objective->getObjectiveType()) {
      case Quest::Objective::Type::KILL: {
        auto objective = dynamic_cast<KillTargetObjective*>(getCurrentStage().objective);
        KillTargetObjective::removeRelatedObjective(objective->getCharacterName(), objective);
        break;
      }
      default: {
        break;
      }
    }
  }

  _currentStageIdx++;

  if (!isCompleted()) {
    switch (getCurrentStage().objective->getObjectiveType()) {
      case Quest::Objective::Type::KILL: {
        auto objective = dynamic_cast<KillTargetObjective*>(getCurrentStage().objective);
        KillTargetObjective::addRelatedObjective(objective->getCharacterName(), objective);
        break;
      }
      default: {
        break;
      }
    }
  }
}

bool Quest::isUnlocked() const {
  return _isUnlocked;
}

bool Quest::isCompleted() const {
  return _currentStageIdx == (int) _questProfile.stages.size();
}

const Quest::Profile& Quest::getQuestProfile() const {
  return _questProfile;
}

const Quest::Stage& Quest::getCurrentStage() const {
  return _questProfile.stages.at(_currentStageIdx);
}




unordered_map<string, vector<Quest::Objective*>> Quest::Objective::_relatedObjectives;
const vector<Quest::Objective*> Quest::Objective::_kEmptyVector(0);

Quest::Objective::Objective(Objective::Type objectiveType, const string& desc)
    : _objectiveType(objectiveType), _desc(desc) {}

Quest::Objective::Type Quest::Objective::getObjectiveType() const {
  return _objectiveType;
}

const string& Quest::Objective::getDesc() const {
  return _desc;
}

void Quest::Objective::addRelatedObjective(const string& key, Quest::Objective* objective) {
  _relatedObjectives[key].push_back(objective);
}

void Quest::Objective::removeRelatedObjective(const string& key, Quest::Objective* objective) {
  auto& objs = _relatedObjectives[key];
  objs.erase(std::remove(objs.begin(), objs.end(), objective), objs.end());
}

const vector<Quest::Objective*>& Quest::Objective::getRelatedObjectives(const string& key) {
  if (_relatedObjectives.find(key) == _relatedObjectives.end()) {
    return _kEmptyVector;
  }
  return _relatedObjectives.at(key);
}




Quest::Stage::Stage(Quest::Objective* objective) : objective(objective) {}


Quest::Profile::Profile(const string& jsonFileName) : jsonFileName(jsonFileName) {
  Document json = json_util::parseJson(jsonFileName);

  title = json["title"].GetString();
  desc = json["desc"].GetString();
  
  for (const auto& stageJson : json["stages"].GetArray()) {
    Quest::Objective::Type objectiveType = static_cast<Quest::Objective::Type>(stageJson["objective"]["objectiveType"].GetInt());
    string objectiveDesc = stageJson["objective"]["desc"].GetString();

    switch (objectiveType) {
      case Quest::Objective::Type::KILL: {
        string characterName = stageJson["objective"]["characterName"].GetString();
        int targetAmount = stageJson["objective"]["targetAmount"].GetInt();
        Objective* objective = new KillTargetObjective(objectiveDesc, characterName, targetAmount);
        stages.push_back(Stage(objective));
        break;
      }
      case Quest::Objective::Type::COLLECT: {
        string itemName = stageJson["objective"]["itemName"].GetString();
        int amount = stageJson["objective"]["amount"].GetInt();
        Objective* objective = new CollectItemObjective(objectiveDesc, itemName, amount);
        stages.push_back(Stage(objective));
        break;
      }
      case Quest::Objective::Type::ESCORT: {
        break;
      }
      case Quest::Objective::Type::DELIVERY: {
        break;
      }
      case Quest::Objective::Type::TALK_TO: {
        break;
      }
      default: {
        break;
      }
    }
  }
}

} // namespace vigilante
