// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Skill.h"

#include <cocos2d.h>
#include <json/document.h>
#include "skill/BackDash.h"
#include "skill/ForwardSlash.h"
#include "skill/MagicalMissile.h"
#include "util/JsonUtil.h"

using std::string;
using rapidjson::Document;

namespace vigilante {

Skill* Skill::create(const string& jsonFileName, Character* user) {
  if (jsonFileName.find("back_dash") != jsonFileName.npos) {
    return new BackDash(jsonFileName, user);
  } else if (jsonFileName.find("forward_slash") != jsonFileName.npos) {
    return new ForwardSlash(jsonFileName, user);
  } else if (jsonFileName.find("ice_spike") != jsonFileName.npos) {
    return new MagicalMissile(jsonFileName, user);
  } else {
    return nullptr;
  }
}


Skill::Profile::Profile(const string& jsonFileName) : jsonFileName(jsonFileName), hotkey() {
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

}  // namespace vigilante
