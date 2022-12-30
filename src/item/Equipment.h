// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_EQUIPMENT_H_
#define VIGILANTE_EQUIPMENT_H_

#include <array>
#include <string>

#include "Item.h"

namespace vigilante {

class Equipment : public Item {
 public:
  enum Type {
    WEAPON,     // 0
    HEADGEAR,   // 1
    ARMOR,      // 2
    GAUNTLETS,  // 3
    BOOTS,      // 4
    CAPE,       // 5
    RING,       // 6
    SIZE
  };

  struct Profile final {
    explicit Profile(const std::string& jsonFileName);
    ~Profile() = default;

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

  static const std::array<std::string, Equipment::Type::SIZE> _kEquipmentTypeStr;

  explicit Equipment(const std::string& jsonFileName);
  virtual ~Equipment() = default;
  virtual void import(const std::string& jsonFileName) override;  // Importable

  inline Equipment::Profile& getEquipmentProfile() { return _equipmentProfile; }

 private:
  Equipment::Profile _equipmentProfile;
};

}  // namespace vigilante

#endif  // VIGILANTE_EQUIPMENT_H_
