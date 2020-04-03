// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_TALK_TO_TARGET_OBJECTIVE_H_
#define VIGILANTE_TALK_TO_TARGET_OBJECTIVE_H_

#include <string>

#include "Quest.h"

namespace vigilante {

class TalkToTargetObjective : public Quest::Objective {
 public:
  explicit TalkToTargetObjective(const std::string& desc);
  virtual ~TalkToTargetObjective() = default;

  virtual bool isCompleted() const override;
};

} // namespace vigilante

#endif // VIGILANTE_TALK_TO_TARGET_OBJECTIVE_H_
