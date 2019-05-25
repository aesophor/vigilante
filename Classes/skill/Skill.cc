#include "Skill.h"

#include "cocos2d.h"
#include "json/document.h"

#include "util/JsonUtil.h"

using std::string;
using rapidjson::Document;

namespace vigilante {

Skill::Profile::Profile(const string& jsonFileName) {
  Document json = json_util::parseJson(jsonFileName);

  characterFramesName = json["characterFramesName"].GetString();
  framesDuration = json["framesDuration"].GetFloat();
  frameInterval = json["frameInterval"].GetFloat();

  textureResDir = json["textureResDir"].GetString();
  name = json["name"].GetString();
  desc = json["desc"].GetString();

  requiredLevel = json["requiredLevel"].GetInt();
  cooldown = json["cooldown"].GetFloat();

  physicalDamage = json["physicalDamage"].GetInt();
  magicalDamage = json["magicalDamage"].GetInt();

  deltaHealth = json["deltaHealth"].GetInt();
  deltaMagicka = json["deltaMagicka"].GetInt();
  deltaStamina = json["deltaStamina"].GetInt();
}

} // namespace vigilante
