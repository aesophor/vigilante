// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_KILL_TARGET_OBJECTIVE_H_
#define VIGILANTE_KILL_TARGET_OBJECTIVE_H_

#include <string>

#include "Quest.h"

namespace vigilante {

class KillTargetObjective : public Quest::Objective {
 public:
  KillTargetObjective(const std::string& desc,
                      const std::string& characterName,
                      int amount=1);
  virtual ~KillTargetObjective() = default;

  virtual bool isCompleted() const override;

  const std::string& getCharacterName() const;
  int getAmount() const;

 private:
  std::string _characterName;
  int _amount;
};

} // namespace vigilante

#endif // VIGILANTE_KILL_TARGET_OBJECTIVE_H_
