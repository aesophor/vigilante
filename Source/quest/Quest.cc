// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "Quest.h"

#include <algorithm>

#include "quest/CollectItemObjective.h"
#include "quest/GeneralObjective.h"
#include "quest/InteractWithTargetObjective.h"
#include "quest/KillTargetObjective.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "ui/console/Console.h"
#include "util/JsonUtil.h"
#include "util/StringUtil.h"

using namespace std;

namespace vigilante {

void Quest::import(const string& jsonFilePath) {
  _questProfile = Quest::Profile(jsonFilePath);
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

string Quest::Stage::getHint() const {
  switch (objective->getObjectiveType()) {
    case Quest::Objective::Type::GENERAL: {
      auto o = dynamic_cast<GeneralObjective*>(objective.get());
      return o->getDesc();
    }
    case Quest::Objective::Type::KILL: {
      auto o = dynamic_cast<KillTargetObjective*>(objective.get());
      return string_util::format("Eliminate: %s (%d/%d)",
          o->getCharacterName().c_str(), o->getCurrentAmount(), o->getTargetAmount());
    }
    case Quest::Objective::Type::COLLECT: {
      auto o = dynamic_cast<CollectItemObjective*>(objective.get());
      // FIXME: show the actual amount of items collected
      return string_util::format("Collect: %s (%d/%d)",
          o->getItemJsonFilePath().c_str(), 0, o->getAmount());
    }
    /*
    case Quest::Objective::Type::ESCORT:
      break;
    case Quest::Objective::Type::DELIVERY:
      break;
    */
    case Quest::Objective::Type::INTERACT_WITH: {
      auto o = dynamic_cast<InteractWithTargetObjective*>(objective.get());
      return string_util::format("Talk to %s", o->getTargetProfileJsonFilePath().c_str());
    }
    default:
      return "";
  }
}

Quest::Profile::Profile(const string& jsonFilePath) : jsonFilePath{jsonFilePath} {
  rapidjson::Document json = json_util::loadFromFile(jsonFilePath);
  title = json["title"].GetString();
  desc = json["desc"].GetString();

  for (const auto& stageJson : json["stages"].GetArray()) {
    Stage stage;
    const auto objectiveType = static_cast<Quest::Objective::Type>(stageJson["objective"]["objectiveType"].GetInt());
    const auto objectiveDesc = stageJson["objective"]["desc"].GetString();
    switch (objectiveType) {
      case Quest::Objective::Type::GENERAL: {
        stage.objective = std::make_unique<GeneralObjective>(objectiveDesc);
        break;
      }
      case Quest::Objective::Type::KILL: {
        const string characterName = stageJson["objective"]["targetJsonFilePath"].GetString();
        const int targetAmount = stageJson["objective"]["targetAmount"].GetInt();
        stage.objective = std::make_unique<KillTargetObjective>(objectiveDesc, characterName, targetAmount);
        break;
      }
      case Quest::Objective::Type::COLLECT: {
        const string itemJsonFilePath = stageJson["objective"]["itemJsonFilePath"].GetString();
        const int amount = stageJson["objective"]["amount"].GetInt();
        stage.objective = std::make_unique<CollectItemObjective>(objectiveDesc, itemJsonFilePath, amount);
        break;
      }
      case Quest::Objective::Type::ESCORT: {
        break;
      }
      case Quest::Objective::Type::DELIVERY: {
        break;
      }
      case Quest::Objective::Type::INTERACT_WITH: {
        const string targetJsonFilePath = stageJson["objective"]["targetJsonFilePath"].GetString();
        stage.objective = std::make_unique<InteractWithTargetObjective>(objectiveDesc, targetJsonFilePath);
        break;
      }
      default: {
        break;
      }
    }

    if (stageJson.HasMember("questDesc")) {
      stage.questDesc = stageJson["questDesc"].GetString();;
    }

    for (const auto& cmd : stageJson["exec"].GetArray()) {
      stage.cmds.push_back(cmd.GetString());
    }

    stages.emplace_back(std::move(stage));
  }
}

}  // namespace vigilante
