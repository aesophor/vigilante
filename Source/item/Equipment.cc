// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "Equipment.h"

#include "util/JsonUtil.h"
#include "util/Logger.h"

namespace fs = std::filesystem;
using namespace std;

namespace requiem {

Equipment::Equipment(const fs::path& jsonFilePath)
    : Item{jsonFilePath},
      _equipmentProfile{jsonFilePath} {}

void Equipment::import(const fs::path& jsonFilePath) {
  Item::import(jsonFilePath);
  _equipmentProfile = Equipment::Profile{jsonFilePath};
}

Equipment::Profile::Profile(const fs::path& jsonFilePath) {
  rapidjson::Document json = json_util::loadFromFile(jsonFilePath);

  equipmentType = static_cast<Equipment::Type>(json["equipmentType"].GetInt());
  bonusPhysicalDamage = json["bonusPhysicalDamage"].GetInt();
  bonusMagicalDamage = json["bonusMagicalDamage"].GetInt();

  bonusStr = json["bonusStr"].GetInt();
  bonusDex = json["bonusDex"].GetInt();
  bonusInt = json["bonusInt"].GetInt();
  bonusLuk = json["bonusLuk"].GetInt();

  bonusMoveSpeed = json["bonusMoveSpeed"].GetInt();
  bonusJumpHeight = json["bonusJumpHeight"].GetInt();

  for (int i = 0; i < Equipment::Sfx::SFX_SIZE; i++) {
    const string& sfxKey = Equipment::_kEquipmentSfxStr[i];
    if (!json["sfx"].HasMember(sfxKey.c_str())) {
      continue;
    }

    const fs::path sfxPath = json["sfx"][sfxKey.c_str()].GetString();
    std::error_code ec;
    if (!fs::exists(sfxPath, ec)) {
      VGLOG(LOG_ERR, "sfx [%s] file doesnt exist.", sfxPath.c_str());
      continue;
    }
    sfxFilePaths[i] = sfxPath;
  }
}

}  // namespace requiem
