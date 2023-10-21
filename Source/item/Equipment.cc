// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Equipment.h"

#include "util/JsonUtil.h"
#include "util/Logger.h"

using namespace std;

namespace vigilante {

Equipment::Equipment(const string& jsonFileName)
    : Item{jsonFileName},
      _equipmentProfile{jsonFileName} {}

void Equipment::import(const string& jsonFileName) {
  Item::import(jsonFileName);
  _equipmentProfile = Equipment::Profile{jsonFileName};
}

Equipment::Profile::Profile(const string& jsonFileName) {
  rapidjson::Document json = json_util::parseJson(jsonFileName);

  for (int i = 0; i < Equipment::Sfx::SFX_SIZE; i++) {
    const string &sfxKey = Equipment::_kEquipmentSfxStr[i];
    if (!json["sfx"].HasMember(sfxKey.c_str())) {
      continue;
    }

    const fs::path sfxPath = json["sfx"][sfxKey.c_str()].GetString();
    std::error_code ec;
    if (!fs::exists(sfxPath, ec)) {
      VGLOG(LOG_ERR, "sfx [%s] file doesnt exist.", sfxPath.c_str());
      continue;
    }
    sfxFileNames[i] = sfxPath;
  }

  equipmentType = static_cast<Equipment::Type>(json["equipmentType"].GetInt());
  bonusPhysicalDamage = json["bonusPhysicalDamage"].GetInt();
  bonusMagicalDamage = json["bonusMagicalDamage"].GetInt();

  bonusStr = json["bonusStr"].GetInt();
  bonusDex = json["bonusDex"].GetInt();
  bonusInt = json["bonusInt"].GetInt();
  bonusLuk = json["bonusLuk"].GetInt();

  bonusMoveSpeed = json["bonusMoveSpeed"].GetInt();
  bonusJumpHeight = json["bonusJumpHeight"].GetInt();
}

}  // namespace vigilante
