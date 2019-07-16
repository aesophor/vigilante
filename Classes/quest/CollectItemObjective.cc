// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "CollectItemObjective.h"

#include <vector>

#include "character/Player.h"
#include "map/GameMapManager.h"

using std::string;
using std::vector;

namespace vigilante {

CollectItemObjective::CollectItemObjective(Quest* quest, const string& desc, Item* item, int amount)
    : Quest::Objective(quest, Quest::Objective::Type::COLLECT, desc),
      _item(item),
      _amount(amount) {}


bool CollectItemObjective::isCompleted() const {
  Player* player = dynamic_cast<Player*>(GameMapManager::getInstance()->getPlayer());
  const vector<Item*>& items = player->getInventory()[_item->getItemProfile().itemType];

  auto it = std::find(items.begin(), items.end(), _item);
  return it == items.end() || (*it)->getAmount() < _amount;
}

Item* CollectItemObjective::getItem() const {
  return _item;
}

int CollectItemObjective::getAmount() const {
  return _amount;
}

} // namespace vigilante
