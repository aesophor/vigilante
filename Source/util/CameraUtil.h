// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_CAMERA_UTIL_H_
#define VIGILANTE_CAMERA_UTIL_H_

#include <axmol.h>
#include <box2d/box2d.h>

#include "map/GameMap.h"

namespace vigilante {

namespace camera_util {

// Camera following a character
void boundCamera(ax::Camera* camera, const GameMap* gameMap);
void lerpToTarget(ax::Camera* camera, const b2Vec2& target);

// Camera shake
void shake(float rumblePower, float rumbleDuration);
void updateShake(ax::Camera* camera, float delta);

} // namespace camera_util

} // namespace vigilante

#endif // VIGILANTE_CAMERA_UTIL_H_
