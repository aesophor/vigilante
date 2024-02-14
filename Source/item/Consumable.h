// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_ITEM_CONSUMABLE_H_
#define VIGILANTE_ITEM_CONSUMABLE_H_

#include <string>

#include <axmol.h>

#include "item/Item.h"
#include "input/Keybindable.h"

namespace vigilante {

class Consumable : public Item, public Keybindable {
 public:
  struct Profile final {
    explicit Profile(const std::string& jsonFilePath);

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

  explicit Consumable(const std::string& jsonFilePath);
  virtual ~Consumable() override = default;

  virtual void import (const std::string& jsonFilePath) override;  // Importable

  virtual ax::EventKeyboard::KeyCode getHotkey() const override;  // Keybindable
  virtual void setHotkey(ax::EventKeyboard::KeyCode hotkey) override;  // Keybindable

  Consumable::Profile& getConsumableProfile() { return _consumableProfile; }

 protected:
  Consumable::Profile _consumableProfile;
};

}  // namespace vigilante

#endif  // VIGILANTE_ITEM_CONSUMABLE_H_
