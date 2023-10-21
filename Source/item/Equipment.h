// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_EQUIPMENT_H_
#define VIGILANTE_EQUIPMENT_H_

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
    explicit Profile(const std::string& jsonFileName);

    std::array<std::string, Equipment::Sfx::SFX_SIZE> sfxFileNames;

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

  explicit Equipment(const std::string& jsonFileName);
  virtual ~Equipment() override = default;

  virtual void import(const std::string& jsonFileName) override;  // Importable

  inline Equipment::Profile& getEquipmentProfile() { return _equipmentProfile; }
  inline const Equipment::Profile& getEquipmentProfile() const {
    return _equipmentProfile;
  }

  const std::string& getSfxFileName(const Equipment::Sfx sfx) const {
    return _equipmentProfile.sfxFileNames[sfx];
  }

 private:
  static inline const std::array<std::string, Equipment::Sfx::SFX_SIZE> _kEquipmentSfxStr{{
    "swing",
    "hit",
  }};

  Equipment::Profile _equipmentProfile;
};

}  // namespace vigilante

#endif  // VIGILANTE_EQUIPMENT_H_
