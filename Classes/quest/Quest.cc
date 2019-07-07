#include "Quest.h"

#include "json/document.h"

#include "util/JsonUtil.h"

using std::string;
using rapidjson::Document;

namespace vigilante {

Quest::Quest(const string& jsonFileName)
    : _questProfile(jsonFileName),
      _currentStageIdx() {}


void Quest::import(const string& jsonFileName) {
  _questProfile = Quest::Profile(jsonFileName);
}

void Quest::advanceStage() {
  if (_currentStageIdx < (int) _questProfile.stages.size()) {
    _currentStageIdx++;
  }
}

bool Quest::isCompleted() const {
  return _currentStageIdx == (int) _questProfile.stages.size();
}

const Quest::Profile& Quest::getQuestProfile() const {
  return _questProfile;
}


Quest::Stage::Stage(const string& objective) : objective(objective) {}

Quest::Profile::Profile(const string& jsonFileName) : jsonFileName(jsonFileName) {
  Document json = json_util::parseJson(jsonFileName);

  title = json["title"].GetString();
  desc = json["desc"].GetString();
  
  for (const auto& stageJson : json["stages"].GetArray()) {
    string objective = stageJson["objective"].GetString();
    Stage stage = Stage(objective);
    
    if (stageJson.HasMember("questDesc")) {
      stage.questDesc = stageJson["questDesc"].GetString();
    }

    stages.push_back(stage);
  }
}

} // namespace vigilante
