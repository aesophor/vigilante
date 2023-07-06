// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_QUEST_HINT_MANAGER_H_
#define VIGILANTE_QUEST_HINT_MANAGER_H_

#include "ui/TimedLabelService.h"

namespace vigilante {

class QuestHints final : public TimedLabelService {
 public:
  QuestHints();
  virtual ~QuestHints() override = default;
};

}  // namespace vigilante

#endif  // VIGILANTE_QUEST_HINT_MANAGER_H_
