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
                       int amount=1)
      : Quest::Objective{Quest::Objective::Type::COLLECT, desc},
        _itemJsonFileName{itemJsonFileName},
        _amount{amount} {}
  virtual ~CollectItemObjective() = default;

  virtual bool isCompleted() const override;

  inline const std::string& getItemJsonFileName() const { return _itemJsonFileName; }
  inline int getAmount() const { return _amount; }

 private:
  const std::string _itemJsonFileName;
  const int _amount;
};

}  // namespace vigilante

#endif  // VIGILANTE_COLLECT_ITEM_OBJECTIVE_H_
