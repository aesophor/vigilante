// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_ITEM_EQUIPMENT_H_
#define VIGILANTE_ITEM_EQUIPMENT_H_

#include <array>
#include <string>

#include "Item.h"

namespace vigilante {

class Equipment : public Item {
 public:
  enum Type {
    WEAPON,
    HEADGEAR,
    ARMOR,
    GAUNTLETS,
    BOOTS,
    CAPE,
    RING,
    SIZE
  };

  enum Sfx {
    SFX_SWING,
    SFX_HIT,
    SFX_SIZE
  };

  static inline const std::array<std::string, Equipment::Type::SIZE> kEquipmentTypeStr{{
    "WEAPON",
    "HEADGEAR",
    "ARMOR",
    "GAUNTLETS",
    "BOOTS",
    "CAPE",
    "RING"
  }};

  struct Profile final {
    explicit Profile(const std::string& jsonFilePath);

    std::array<std::string, Equipment::Sfx::SFX_SIZE> sfxFilePaths;

    Equipment::Type equipmentType;
    int bonusPhysicalDamage;
    int bonusMagicalDamage;

    int bonusStr;
    int bonusDex;
    int bonusInt;
    int bonusLuk;

    int bonusMoveSpeed;
    int bonusJumpHeight;
  };

  explicit Equipment(const std::string& jsonFilePath);
  virtual ~Equipment() override = default;

  virtual void import(const std::string& jsonFilePath) override;  // Importable

  inline Equipment::Profile& getEquipmentProfile() { return _equipmentProfile; }
  inline const Equipment::Profile& getEquipmentProfile() const {
    return _equipmentProfile;
  }

  const std::string& getSfxFilePath(const Equipment::Sfx sfx) const {
    return _equipmentProfile.sfxFilePaths[sfx];
  }

 private:
  static inline const std::array<std::string, Equipment::Sfx::SFX_SIZE> _kEquipmentSfxStr{{
    "swing",
    "hit",
  }};

  Equipment::Profile _equipmentProfile;
};

}  // namespace vigilante

#endif  // VIGILANTE_ITEM_EQUIPMENT_H_
