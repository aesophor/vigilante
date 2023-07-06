// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Quest.h"

#include <algorithm>

#include "quest/CollectItemObjective.h"
#include "quest/KillTargetObjective.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "ui/console/Console.h"
#include "util/JsonUtil.h"
#include "util/StringUtil.h"

using namespace std;

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

  // Execute the commands that are supposed to run after
  // this stage is completed.
  if (_currentStageIdx >= 0) {
    auto console = SceneManager::the().getCurrentScene<GameScene>()->getConsole();
    for (const auto& cmd : getCurrentStage().cmds) {
      console->executeCmd(cmd);
    }
  }
  ++_currentStageIdx;

  // Update quest desc (if provided).
  // We can only update quest desc if it hasn't been completed,
  // or we'll get out of index during `getCurrentStage()`.
  if (!isCompleted() && !getCurrentStage().questDesc.empty()) {
    _questProfile.desc = getCurrentStage().questDesc;
  }
}

Quest::Objective::Objective(Objective::Type objectiveType, const string& desc)
    : _objectiveType(objectiveType), _desc(desc) {}

string Quest::Stage::getHint() const {
  switch (objective->getObjectiveType()) {
    case Quest::Objective::Type::KILL: {
      KillTargetObjective* o = dynamic_cast<KillTargetObjective*>(objective.get()); 
      return string_util::format("Eliminate: %s (%d/%d)",
          o->getCharacterName().c_str(), o->getCurrentAmount(), o->getTargetAmount());
    }
    case Quest::Objective::Type::COLLECT: {
      CollectItemObjective* o = dynamic_cast<CollectItemObjective*>(objective.get());
      // FIXME: show the actual amount of items collected
      return string_util::format("Collect: %s (%d/%d)",
          o->getItemJsonFileName().c_str(), 0, o->getAmount());
    }
    /*
    case Quest::Objective::Type::ESCORT:
      break;
    case Quest::Objective::Type::DELIVERY:
      break;
    case Quest::Objective::Type::TALK_TO:
      break;
    case Quest::Objective::Type::AD_HOC:
      break;
    */
    default:
      return "";
  }
}

Quest::Profile::Profile(const string& jsonFileName) : jsonFileName(jsonFileName) {
  rapidjson::Document json = json_util::parseJson(jsonFileName);

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
        string itemJsonFileName = stageJson["objective"]["itemJsonFileName"].GetString();
        int amount = stageJson["objective"]["amount"].GetInt();
        stage.objective = std::make_unique<CollectItemObjective>(objectiveDesc, itemJsonFileName, amount);
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

    for (const auto& cmd : stageJson["exec"].GetArray()) {
      stage.cmds.push_back(cmd.GetString());
    }

    stages.push_back(std::move(stage));
  }
}

}  // namespace vigilante
