// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "NotificationManager.h"

#define STARTING_X 10.0f
#define STARTING_Y 10.0f
#define MAX_LABEL_COUNT 10
#define LABEL_LIFETIME 5.0f
#define LABEL_ALIGNMENT TimedLabelService::TimedLabel::kLeft

namespace vigilante {

NotificationManager* NotificationManager::_instance = nullptr;

NotificationManager* NotificationManager::getInstance() {
  if (!_instance) {
    _instance = new NotificationManager();
  }
  return _instance;
}

NotificationManager::NotificationManager()
    : TimedLabelService(STARTING_X, STARTING_Y, MAX_LABEL_COUNT, LABEL_LIFETIME, LABEL_ALIGNMENT) {}

} // namespace vigilante
