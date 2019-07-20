// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "QuestHintManager.h"

#define STARTING_X 300.0f
#define STARTING_Y 200.0f
#define MAX_LABEL_COUNT 10
#define LABEL_LIFETIME 8.0f
#define LABEL_ALIGNMENT TimedLabelService::TimedLabel::kCenter

namespace vigilante {

QuestHintManager* QuestHintManager::_instance = nullptr;

QuestHintManager* QuestHintManager::getInstance() {
  if (!_instance) {
    _instance = new QuestHintManager();
  }
  return _instance;
}

QuestHintManager::QuestHintManager()
    : TimedLabelService(STARTING_X, STARTING_Y, MAX_LABEL_COUNT, LABEL_LIFETIME, LABEL_ALIGNMENT) {}

} // namespace vigilante
