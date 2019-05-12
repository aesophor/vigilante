#include "Equipment.h"

using std::string;

namespace vigilante {

Equipment::Equipment(const Equipment::Type equipmentType,
                     const string& name,
                     const string& desc,
                     const string& imgPath,
                     float x,
                     float y)
    : Item(Item::Type::EQUIPMENT, name, desc, imgPath, x, y),
      _equipmentType(equipmentType) {}

} // namespace vigilante
