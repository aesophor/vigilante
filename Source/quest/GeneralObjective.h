// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_QUEST_GENERAL_OBJECTIVE_H_
#define REQUIEM_QUEST_GENERAL_OBJECTIVE_H_

#include <string>

#include "Quest.h"

namespace requiem {

// This type of quest objective can only be advanced (i.e. completed)
// through console command.
class GeneralObjective : public Quest::Objective {
 public:
  GeneralObjective(const std::string& desc) : Quest::Objective{Quest::Objective::Type::GENERAL, desc} {}

  virtual bool isCompleted() const override { return false; }
};

}  // namespace requiem

#endif  // REQUIEM_QUEST_GENERAL_OBJECTIVE_H_
