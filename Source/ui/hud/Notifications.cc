// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "Notifications.h"

namespace vigilante {

namespace {

constexpr float kStartingX = 10.0f;
constexpr float kStartingY = 25.0f;
constexpr int kMaxLabelCount = 10;
constexpr uint8_t kMaxLabelLifetime = 5;

}  // namespace

Notifications::Notifications()
    : TimedLabelService{kStartingX, kStartingY, kMaxLabelCount, kMaxLabelLifetime, TimedLabelService::TimedLabel::kLeft} {}

}  // namespace vigilante
