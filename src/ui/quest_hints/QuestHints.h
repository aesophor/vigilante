// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_QUEST_HINT_MANAGER_H_
#define VIGILANTE_QUEST_HINT_MANAGER_H_

#include "ui/TimedLabelService.h"

namespace vigilante {

class QuestHints : public TimedLabelService {
 public:
  static QuestHints* getInstance();
  virtual ~QuestHints() = default;

 private:
  QuestHints();
};

}  // namespace vigilante

#endif  // VIGILANTE_QUEST_HINT_MANAGER_H_
