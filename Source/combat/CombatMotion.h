// Copyright (c) 2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_COMBAT_MOTION_H_
#define VIGILANTE_COMBAT_MOTION_H_

#include "character/Character.h"

namespace vigilante {

bool handleCombatMotion(Character& c, const Character::State attackState);

}  // namespace vigilante

#endif  // VIGILANTE_COMBAT_MOTION_H_
