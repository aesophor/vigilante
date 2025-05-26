// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_QUEST_COLLECT_ITEM_OBJECTIVE_H_
#define REQUIEM_QUEST_COLLECT_ITEM_OBJECTIVE_H_

#include <string>

#include "Quest.h"

namespace requiem {

class CollectItemObjective : public Quest::Objective {
 public:
  CollectItemObjective(const std::string& desc,
                       const std::string& itemJsonFilePath,
                       int amount=1)
      : Quest::Objective{Quest::Objective::Type::COLLECT, desc},
        _itemJsonFilePath{itemJsonFilePath},
        _amount{amount} {}
  virtual ~CollectItemObjective() = default;

  virtual bool isCompleted() const override;

  inline const std::string& getItemJsonFilePath() const { return _itemJsonFilePath; }
  inline int getAmount() const { return _amount; }

 private:
  const std::string _itemJsonFilePath;
  const int _amount;
};

}  // namespace requiem

#endif  // REQUIEM_QUEST_COLLECT_ITEM_OBJECTIVE_H_
