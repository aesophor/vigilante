#ifndef VIGILANTE_CONSUMABLE_H_
#define VIGILANTE_CONSUMABLE_H_

#include <string>

#include "item/Item.h"

namespace vigilante {

class Consumable : public Item {
 public:
  enum Type {
    POTION,
    SIZE
  };

  struct Profile {
    Profile(const std::string& jsonFileName);
    virtual ~Profile() = default;

    Consumable::Type consumableType;
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
  };

  Consumable();
  virtual ~Consumable() = default;

 protected:

};

} // namespace vigilante

#endif // VIGILANTE_CONSUMABLE_H_
