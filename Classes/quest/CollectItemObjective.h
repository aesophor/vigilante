// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_COLLECT_ITEM_OBJECTIVE_H_
#define VIGILANTE_COLLECT_ITEM_OBJECTIVE_H_

#include <string>

#include "Quest.h"
#include "item/Item.h"

namespace vigilante {

class CollectItemObjective : public Quest::Objective {
 public:
  CollectItemObjective(Quest* quest, const std::string& desc, Item* item, int amount);
  virtual ~CollectItemObjective() = default;

  virtual bool isCompleted() const override;
  Item* getItem() const;
  int getAmount() const;

 private:
  Item* _item;
  int _amount;
};

} // namespace vigilante

#endif // VIGILANTE_COLLECT_ITEM_OBJECTIVE_H_
