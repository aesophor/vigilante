// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_QUEST_HINT_MANAGER_H_
#define VIGILANTE_QUEST_HINT_MANAGER_H_

#include "ui/TimedLabelService.h"

namespace vigilante {

class QuestHintManager : public TimedLabelService {
 public:
  static QuestHintManager* getInstance();
  virtual ~QuestHintManager() = default;

 private:
  static QuestHintManager* _instance;
  QuestHintManager();
};

} // namespace vigilante

#endif // VIGILANTE_QUEST_HINT_MANAGER_H_
