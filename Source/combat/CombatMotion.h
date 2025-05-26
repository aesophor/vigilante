// Copyright (c) 2023-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_COMBAT_COMBAT_MOTION_H_
#define REQUIEM_COMBAT_COMBAT_MOTION_H_

#include "character/Character.h"

namespace requiem {

bool handleCombatMotion(Character& c, const Character::State attackState);

}  // namespace requiem

#endif  // REQUIEM_COMBAT_COMBAT_MOTION_H_
