// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_QUEST_GENERAL_OBJECTIVE_H_
#define VIGILANTE_QUEST_GENERAL_OBJECTIVE_H_

#include <string>

#include "Quest.h"

namespace vigilante {

// This type of quest objective can only be advanced (i.e. completed)
// through console command.
class GeneralObjective : public Quest::Objective {
 public:
  GeneralObjective(const std::string& desc) : Quest::Objective{Quest::Objective::Type::GENERAL, desc} {}

  virtual bool isCompleted() const override { return false; }
};

}  // namespace vigilante

#endif  // VIGILANTE_QUEST_GENERAL_OBJECTIVE_H_
