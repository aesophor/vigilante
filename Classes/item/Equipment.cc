#include "Equipment.h"

using std::string;

namespace vigilante {

Equipment::Equipment(const EquipmentType equipmentType,
                     const string& name,
                     const string& desc,
                     const string& imgPath,
                     float x,
                     float y)
    : Item(ItemType::EQUIPMENT, name, desc, imgPath, x, y),
      _equipmentType(equipmentType) {}

} // namespace vigilante
