// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "CollectItemObjective.h"

#include <vector>

#include "character/Player.h"
#include "item/Item.h"
#include "map/GameMapManager.h"

using std::string;
using std::vector;

namespace vigilante {

CollectItemObjective::CollectItemObjective(const string& desc,
                                           const string& itemName,
                                           int amount)
    : Quest::Objective(Quest::Objective::Type::COLLECT, desc),
      _itemName(itemName),
      _amount(amount) {}


bool CollectItemObjective::isCompleted() const {
  return GameMapManager::getInstance()->getPlayer()->getItemAmount(_itemName) >= _amount;
}

const string& CollectItemObjective::getItemName() const {
  return _itemName;
}

int CollectItemObjective::getAmount() const {
  return _amount;
}

} // namespace vigilante
