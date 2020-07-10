// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "QuestHints.h"

#include "Constants.h"

#define STARTING_X vigilante::kVirtualWidth / 2
#define STARTING_Y vigilante::kVirtualHeight * .75f
#define MAX_LABEL_COUNT 10
#define LABEL_LIFETIME 8.0f
#define LABEL_ALIGNMENT TimedLabelService::TimedLabel::kCenter

namespace vigilante {

QuestHints* QuestHints::getInstance() {
  static QuestHints instance;
  return &instance;
}

QuestHints::QuestHints()
    : TimedLabelService(STARTING_X, STARTING_Y, MAX_LABEL_COUNT, LABEL_LIFETIME, LABEL_ALIGNMENT) {}

} // namespace vigilante
