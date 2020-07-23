// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_COLLECT_ITEM_OBJECTIVE_H_
#define VIGILANTE_COLLECT_ITEM_OBJECTIVE_H_

#include <string>

#include "Quest.h"

namespace vigilante {

class CollectItemObjective : public Quest::Objective {
 public:
  CollectItemObjective(const std::string& desc,
                       const std::string& itemName,
                       int amount=1);
  virtual ~CollectItemObjective() = default;

  virtual bool isCompleted() const override;

  const std::string& getItemName() const;
  int getAmount() const;

 private:
  std::string _itemName;
  int _amount;
};

}  // namespace vigilante

#endif  // VIGILANTE_COLLECT_ITEM_OBJECTIVE_H_
