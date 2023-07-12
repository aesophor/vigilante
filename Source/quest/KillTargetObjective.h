// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_KILL_TARGET_OBJECTIVE_H_
#define VIGILANTE_KILL_TARGET_OBJECTIVE_H_

#include <string>

#include "Quest.h"

namespace vigilante {

class KillTargetObjective : public Quest::Objective {
 public:
  KillTargetObjective(const std::string& desc,
                      const std::string& characterName,
                      int targetAmount=1)
      : Quest::Objective{Quest::Objective::Type::KILL, desc},
        _characterName{characterName},
        _targetAmount{targetAmount} {}
  virtual ~KillTargetObjective() = default;

  virtual bool isCompleted() const override {
    return _currentAmount >= _targetAmount;
  }

  inline const std::string& getCharacterName() const { return _characterName; }
  inline int getTargetAmount() const { return _targetAmount; }
  inline int getCurrentAmount() const { return _currentAmount; }
  inline void incrementCurrentAmount() { _currentAmount++; }

 private:
  const std::string _characterName;
  const int _targetAmount;
  int _currentAmount{};
};

}  // namespace vigilante

#endif  // VIGILANTE_KILL_TARGET_OBJECTIVE_H_
