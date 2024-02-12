// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Skill.h"

#include <axmol.h>

#include "skill/BatForm.h"
#include "skill/BeastForm.h"
#include "skill/ForwardSlash.h"
#include "skill/MagicalMissile.h"
#include "skill/TeleportStrike.h"
#include "util/JsonUtil.h"
#include "util/Logger.h"

using namespace std;

namespace vigilante {

shared_ptr<Skill> Skill::create(const string& jsonFilePath, Character* user) {
  if (jsonFilePath.find("bat_form") != jsonFilePath.npos) {
    return make_shared<BatForm>(jsonFilePath, user);
  } else if (jsonFilePath.find("beast_form") != jsonFilePath.npos) {
    return make_shared<BeastForm>(jsonFilePath, user);
  } else if (jsonFilePath.find("forward_slash") != jsonFilePath.npos) {
    return make_shared<ForwardSlash>(jsonFilePath, user);
  } else if (jsonFilePath.find("teleport_strike") != jsonFilePath.npos) {
    return make_shared<TeleportStrike>(jsonFilePath, user);
  } else if (jsonFilePath.find("ice_spike") != jsonFilePath.npos) {
    return make_shared<MagicalMissile>(jsonFilePath, user, /*onGround=*/false);
  } else if (jsonFilePath.find("ice_wave") != jsonFilePath.npos) {
    return make_shared<MagicalMissile>(jsonFilePath, user, /*onGround=*/true);
  }

  VGLOG(LOG_ERR, "Unable to create skill: [%s].", jsonFilePath.c_str());
  return nullptr;
}

Skill::Profile::Profile(const string& jsonFilePath) : jsonFilePath(jsonFilePath), hotkey() {
  rapidjson::Document json = json_util::loadFromFile(jsonFilePath);

  skillType = static_cast<Skill::Type>(json["skillType"].GetInt());
  characterFramesName = json["characterFramesName"].GetString();

  textureResDir = json["textureResDir"].GetString();
  spriteOffsetX = json["spriteOffsetX"].GetFloat();
  spriteOffsetY = json["spriteOffsetY"].GetFloat();
  spriteScaleX = json["spriteScaleX"].GetFloat();
  spriteScaleY = json["spriteScaleY"].GetFloat();
  framesDuration = json["framesDuration"].GetFloat();

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
  numTimesInflictDamage = json["numTimesInflictDamage"].GetInt();
  damageInflictionInterval = json["damageInflictionInterval"].GetFloat();

  sfxActivate = json["sfxActivate"].GetString();
  sfxHit = json["sfxHit"].GetString();
}

}  // namespace vigilante
