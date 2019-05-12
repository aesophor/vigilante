#ifndef VIGILANTE_EQUIPMENT_H_
#define VIGILANTE_EQUIPMENT_H_

#include <string>

#include "Item.h"

namespace vigilante {

enum EquipmentType {
  WEAPON,     // 0
  HEADGEAR,   // 1
  ARMOR,      // 2
  GAUNTLETS,  // 3
  BOOTS,      // 4
  CAPE,       // 5
  RING        // 6
};

class Equipment : public Item {
 public:
  // TODO: load item properties from JSON
  Equipment(const EquipmentType equipmentType,
            const std::string& name,
            const std::string& desc,
            const std::string& imgPath,
            float x,
            float y);
  virtual ~Equipment() = default;

  const EquipmentType getEquipmentType() const;
  int getBonusPhysicalDamage() const;
  int getBonusMagicalDamage() const;
  int getBonusStr() const;
  int getBonusDex() const;
  int getBonusInt() const;
  int getBonusLuk() const;
  int getBonusMovingSpeed() const;
  int getBonusJumpingHeight() const;

  void setBounsPhysicalDamage(int val);
  void setBonusMagicalDamage(int val);
  void setBonusStr(int val);
  void setBonusDex(int val);
  void setBonusInt(int val);
  void setBonusLuk(int val);
  void setBonusMovingSpeed(int val);
  void setBonusJumpingHeight(int val);

 private:
  EquipmentType _equipmentType;
  int _bonusPhysicalDamage;
  int _bonusMagicalDamage;
  int _bonusStr;
  int _bonusDex;
  int _bonusInt;
  int _bonusLuk;
  int _bonusMovingSpeed;
  int _bonusJumpingHeight;
};

} // namespace vigilante

#endif // VIGILANTE_EQUIPMENT_H_
