// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "QuestHints.h"

#include "Constants.h"

namespace requiem {

namespace {

constexpr auto kStartingX = requiem::kVirtualWidth / 2;
constexpr auto kStartingY = requiem::kVirtualHeight * .65f;
constexpr auto kMaxLabelCount = 5;
constexpr auto kMaxLabelLifetime = 8.0f;

}  // namespace

QuestHints::QuestHints()
    : TimedLabelService(kStartingX, kStartingY, kMaxLabelCount,
                        kMaxLabelLifetime, TimedLabelService::TimedLabel::kCenter) {}

}  // namespace requiem
