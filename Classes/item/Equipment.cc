#include "Equipment.h"

using std::array;
using std::string;

namespace vigilante {

const array<string, Equipment::Type::SIZE> Equipment::_kEquipmentTypeStr = {{
  "WEAPON",
  "HEADGEAR",
  "ARMOR",
  "GAUNTLETS",
  "BOOTS",
  "CAPE",
  "RING"
}};

Equipment::Equipment(const Equipment::Type equipmentType,
                     const string& name,
                     const string& desc,
                     const string& imgPath,
                     float x,
                     float y)
    : Item(Item::Type::EQUIPMENT, name, desc, imgPath, x, y),
      _equipmentType(equipmentType) {}


const Equipment::Type Equipment::getEquipmentType() const {
  return _equipmentType;
}

} // namespace vigilante
