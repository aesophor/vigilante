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

namespace fs = std::filesystem;
using namespace std;

namespace requiem {

shared_ptr<Skill> Skill::create(const fs::path& jsonFilePath, Character* user) {
  if (jsonFilePath.native().find("bat_form") != jsonFilePath.native().npos) {
    return make_shared<BatForm>(jsonFilePath, user);
  } else if (jsonFilePath.native().find("beast_form") != jsonFilePath.native().npos) {
    return make_shared<BeastForm>(jsonFilePath, user);
  } else if (jsonFilePath.native().find("forward_slash") != jsonFilePath.native().npos) {
    return make_shared<ForwardSlash>(jsonFilePath, user);
  } else if (jsonFilePath.native().find("teleport_strike") != jsonFilePath.native().npos) {
    return make_shared<TeleportStrike>(jsonFilePath, user);
  } else if (jsonFilePath.native().find("ice_spike") != jsonFilePath.native().npos) {
    return make_shared<MagicalMissile>(jsonFilePath, user, /*onGround=*/false);
  } else if (jsonFilePath.native().find("ice_wave") != jsonFilePath.native().npos) {
    return make_shared<MagicalMissile>(jsonFilePath, user, /*onGround=*/true);
  }

  VGLOG(LOG_ERR, "Unable to create skill: [%s].", jsonFilePath.c_str());
  return nullptr;
}

Skill::Profile::Profile(const fs::path& jsonFilePath) : jsonFilePath(jsonFilePath), hotkey() {
  rapidjson::Document json = json_util::loadFromFile(jsonFilePath);

  skillType = static_cast<Skill::Type>(json["skillType"].GetInt());
  characterFramesName = json["characterFramesName"].GetString();
  textureResDirPath = json["textureResDirPath"].GetString();
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

}  // namespace requiem
