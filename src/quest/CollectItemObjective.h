// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_COLLECT_ITEM_OBJECTIVE_H_
#define VIGILANTE_COLLECT_ITEM_OBJECTIVE_H_

#include <string>

#include "Quest.h"

namespace vigilante {

class CollectItemObjective : public Quest::Objective {
 public:
  CollectItemObjective(const std::string& desc,
                       const std::string& itemJsonFileName,
                       int amount=1);
  virtual ~CollectItemObjective() = default;

  virtual bool isCompleted() const override;

  inline const std::string& getItemJsonFileName() const { return _itemJsonFileName; }
  inline int getAmount() const { return _amount; }

 private:
  std::string _itemJsonFileName;
  int _amount;
};

}  // namespace vigilante

#endif  // VIGILANTE_COLLECT_ITEM_OBJECTIVE_H_
