// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "Consumable.h"

#include "util/JsonUtil.h"

namespace fs = std::filesystem;
using namespace std;
USING_NS_AX;

namespace requiem {

Consumable::Consumable(const fs::path& jsonFilePath)
    : Item{jsonFilePath},
      _consumableProfile{jsonFilePath} {}

void Consumable::import(const fs::path& jsonFilePath) {
  Item::import(jsonFilePath);
  _consumableProfile = Consumable::Profile{jsonFilePath};
}

EventKeyboard::KeyCode Consumable::getHotkey() const {
  return _consumableProfile.hotkey;
}

void Consumable::setHotkey(EventKeyboard::KeyCode hotkey) {
  _consumableProfile.hotkey = hotkey;
}

Consumable::Profile::Profile(const fs::path& jsonFilePath) : hotkey{} {
  rapidjson::Document json = json_util::loadFromFile(jsonFilePath);

  duration = json["duration"].GetFloat();

  restoreHealth = json["restoreHealth"].GetInt();
  restoreMagicka = json["restoreMagicka"].GetInt();
  restoreStamina = json["restoreStamina"].GetInt();

  bonusPhysicalDamage = json["bonusPhysicalDamage"].GetInt();
  bonusMagicalDamage = json["bonusMagicalDamage"].GetInt();

  bonusStr = json["bonusStr"].GetInt();
  bonusDex = json["bonusDex"].GetInt();
  bonusInt = json["bonusInt"].GetInt();
  bonusLuk = json["bonusLuk"].GetInt();

  bonusMoveSpeed = json["bonusMoveSpeed"].GetInt();
  bonusJumpHeight = json["bonusJumpHeight"].GetInt();
}

}  // namespace requiem
