// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Quest.h"

#include <cocos2d.h>
#include <json/document.h>
#include "quest/CollectItemObjective.h"
#include "util/JsonUtil.h"

using std::string;
using rapidjson::Document;

namespace vigilante {

Quest::Quest(const string& jsonFileName)
    : _questProfile(jsonFileName),
      _isUnlocked(),
      _currentStageIdx() {}

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
  if (_currentStageIdx < (int) _questProfile.stages.size()) {
    _currentStageIdx++;
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


Quest::Objective::Objective(Objective::Type objectiveType, const string& desc)
    : _objectiveType(objectiveType), _desc(desc) {}

Quest::Objective::Type Quest::Objective::getObjectiveType() const {
  return _objectiveType;
}

const string& Quest::Objective::getDesc() const {
  return _desc;
}


Quest::Stage::Stage(Quest::Objective* objective) : objective(objective) {}


Quest::Profile::Profile(const string& jsonFileName) : jsonFileName(jsonFileName) {
  Document json = json_util::parseJson(jsonFileName);

  title = json["title"].GetString();
  desc = json["desc"].GetString();
  
  for (const auto& stageJson : json["stages"].GetArray()) {
    Quest::Objective::Type objectiveType = static_cast<Quest::Objective::Type>(stageJson["objective"]["objectiveType"].GetInt());

    switch (objectiveType) {
      case Quest::Objective::Type::KILL: {
        break;
      }
      case Quest::Objective::Type::COLLECT: {
        string objectiveDesc = stageJson["objective"]["desc"].GetString();
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
      default: {
        break;
      }
    }
  }
}

} // namespace vigilante
