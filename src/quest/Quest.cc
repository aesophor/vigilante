// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Quest.h"

#include <algorithm>

#include <json/document.h>
#include "std/make_unique.h"
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
  ++_currentStageIdx;

  if (!isCompleted() && !getCurrentStage().questDesc.empty()) {
    _questProfile.desc = getCurrentStage().questDesc;
  }
}

bool Quest::isUnlocked() const {
  return _isUnlocked;
}

bool Quest::isCompleted() const {
  return _currentStageIdx >= (int) _questProfile.stages.size();
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



Quest::Profile::Profile(const string& jsonFileName) : jsonFileName(jsonFileName) {
  Document json = json_util::parseJson(jsonFileName);

  title = json["title"].GetString();
  desc = json["desc"].GetString();
  
  for (const auto& stageJson : json["stages"].GetArray()) {
    auto objectiveType = static_cast<Quest::Objective::Type>(stageJson["objective"]["objectiveType"].GetInt());
    auto objectiveDesc = stageJson["objective"]["desc"].GetString();

    Stage stage;

    switch (objectiveType) {
      case Quest::Objective::Type::KILL: {
        string characterName = stageJson["objective"]["characterName"].GetString();
        int targetAmount = stageJson["objective"]["targetAmount"].GetInt();
        stage.objective = std::make_unique<KillTargetObjective>(objectiveDesc, characterName, targetAmount);
        break;
      }
      case Quest::Objective::Type::COLLECT: {
        string itemName = stageJson["objective"]["itemName"].GetString();
        int amount = stageJson["objective"]["amount"].GetInt();
        stage.objective = std::make_unique<CollectItemObjective>(objectiveDesc, itemName, amount);
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

    if (stageJson.HasMember("questDesc")) {
      string questDesc = stageJson["questDesc"].GetString();
      stage.questDesc = questDesc;
    }

    stages.push_back(std::move(stage));
  }
}

}  // namespace vigilante
