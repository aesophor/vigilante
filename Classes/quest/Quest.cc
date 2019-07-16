// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Quest.h"

#include "json/document.h"

#include "util/JsonUtil.h"

using std::string;
using rapidjson::Document;

namespace vigilante {

Quest::Quest(const string& jsonFileName)
    : _questProfile(jsonFileName),
      _isUnlocked(),
      _currentStageIdx() {}


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
  return _questProfile.stages[_currentStageIdx];
}


Quest::Objective::Objective(Quest* quest, Objective::Type objectiveType, const string& desc)
    : _quest(quest), _objectiveType(objectiveType), _desc(desc) {}

Quest::Objective::Type Quest::Objective::getObjectiveType() const {
  return _objectiveType;
}

const string& Quest::Objective::getDesc() const {
  return _desc;
}

Quest* Quest::Objective::getQuest() const {
  return _quest;
}


Quest::Stage::Stage(Quest::Objective* objective) : objective(objective) {}

Quest::Stage::~Stage() {
  if (objective) {
    delete objective;
  }
}


Quest::Profile::Profile(const string& jsonFileName) : jsonFileName(jsonFileName) {
  Document json = json_util::parseJson(jsonFileName);

  title = json["title"].GetString();
  desc = json["desc"].GetString();
  
  /*
  for (const auto& stageJson : json["stages"].GetArray()) {
    string objective = stageJson["objective"].GetString();
//    Stage stage = Stage(objective);
    
    if (stageJson.HasMember("questDesc")) {
//      stage.questDesc = stageJson["questDesc"].GetString();
    }

//    stages.push_back(stage);
  }
  */
}

} // namespace vigilante
