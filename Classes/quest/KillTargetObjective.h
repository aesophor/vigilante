// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_KILL_TARGET_OBJECTIVE_H_
#define VIGILANTE_KILL_TARGET_OBJECTIVE_H_

#include <string>

#include "Quest.h"
#include "character/Character.h"

namespace vigilante {

class KillTargetObjective : public Quest::Objective {
 public:
  KillTargetObjective(Quest* quest, const std::string& desc, Character* target);
  virtual ~KillTargetObjective() = default;

  virtual bool isCompleted() const override;
  Character* getTarget() const;

 private:
  Character* _target;
};

} // namespace vigilante

#endif // VIGILANTE_KILL_TARGET_OBJECTIVE_H_
