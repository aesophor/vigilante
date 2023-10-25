// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Skill.h"

#include <axmol.h>

#include "skill/BatForm.h"
#include "skill/BeastForm.h"
#include "skill/ForwardSlash.h"
#include "skill/MagicalMissile.h"
#include "util/JsonUtil.h"
#include "util/Logger.h"

using namespace std;

namespace vigilante {

shared_ptr<Skill> Skill::create(const string& jsonFileName, Character* user) {
  if (jsonFileName.find("bat_form") != jsonFileName.npos) {
    return make_shared<BatForm>(jsonFileName, user);
  } else if (jsonFileName.find("beast_form") != jsonFileName.npos) {
    return make_shared<BeastForm>(jsonFileName, user);
  } else if (jsonFileName.find("forward_slash") != jsonFileName.npos) {
    return make_shared<ForwardSlash>(jsonFileName, user);
  } else if (jsonFileName.find("ice_spike") != jsonFileName.npos) {
    return make_shared<MagicalMissile>(jsonFileName, user, /*onGround=*/false);
  } else if (jsonFileName.find("ice_wave") != jsonFileName.npos) {
    return make_shared<MagicalMissile>(jsonFileName, user, /*onGround=*/true);
  }

  VGLOG(LOG_ERR, "Unable to create skill: [%s].", jsonFileName.c_str());
  return nullptr;
}

Skill::Profile::Profile(const string& jsonFileName) : jsonFileName(jsonFileName), hotkey() {
  rapidjson::Document json = json_util::parseJson(jsonFileName);

  skillType = static_cast<Skill::Type>(json["skillType"].GetInt());
  characterFramesName = json["characterFramesName"].GetString();

  textureResDir = json["textureResDir"].GetString();
  spriteOffsetX = json["spriteOffsetX"].GetFloat();
  spriteOffsetY = json["spriteOffsetY"].GetFloat();
  spriteScaleX = json["spriteScaleX"].GetFloat();
  spriteScaleY = json["spriteScaleY"].GetFloat();
  framesDuration = json["framesDuration"].GetFloat();
  frameInterval = json["frameInterval"].GetFloat();

  name = json["name"].GetString();
  desc = json["desc"].GetString();
  isToggleable = json["isToggleable"].GetBool();
  shouldForkInstance = json["shouldForkInstance"].GetBool();
  requiredLevel = json["requiredLevel"].GetInt();
  cooldown = json["cooldown"].GetFloat();

  physicalDamage = json["physicalDamage"].GetInt();
  magicalDamage = json["magicalDamage"].GetInt();
  deltaHealth = json["deltaHealth"].GetInt();
  deltaMagicka = json["deltaMagicka"].GetInt();
  deltaStamina = json["deltaStamina"].GetInt();

  sfxActivate = json["sfxActivate"].GetString();
  sfxHit = json["sfxHit"].GetString();
}

}  // namespace vigilante
