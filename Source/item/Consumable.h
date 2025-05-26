// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_ITEM_CONSUMABLE_H_
#define REQUIEM_ITEM_CONSUMABLE_H_

#include <string>

#include <axmol.h>

#include "item/Item.h"
#include "input/Keybindable.h"

namespace requiem {

class Consumable : public Item, public Keybindable {
 public:
  struct Profile final {
    explicit Profile(const std::filesystem::path& jsonFilePath);

    float duration; // sec

    int restoreHealth;
    int restoreMagicka;
    int restoreStamina;

    int bonusPhysicalDamage;
    int bonusMagicalDamage;

    int bonusStr;
    int bonusDex;
    int bonusInt;
    int bonusLuk;

    int bonusMoveSpeed;
    int bonusJumpHeight;

    ax::EventKeyboard::KeyCode hotkey;
  };

  explicit Consumable(const std::filesystem::path& jsonFilePath);
  virtual ~Consumable() override = default;

  virtual void import (const std::filesystem::path& jsonFilePath) override;  // Importable

  virtual ax::EventKeyboard::KeyCode getHotkey() const override;  // Keybindable
  virtual void setHotkey(ax::EventKeyboard::KeyCode hotkey) override;  // Keybindable

  Consumable::Profile& getConsumableProfile() { return _consumableProfile; }

 protected:
  Consumable::Profile _consumableProfile;
};

}  // namespace requiem

#endif  // REQUIEM_ITEM_CONSUMABLE_H_
