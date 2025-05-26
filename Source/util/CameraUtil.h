// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_UTIL_CAMERA_UTIL_H_
#define REQUIEM_UTIL_CAMERA_UTIL_H_

#include <axmol.h>
#include <box2d/box2d.h>

#include "map/GameMap.h"

namespace requiem::camera_util {

// Camera following a character
void boundCamera(ax::Camera* camera, const GameMap* gameMap);
void lerpToTarget(ax::Camera* camera, const b2Vec2& target);

// Camera shake
void shake(float rumblePower, float rumbleDuration);
void updateShake(ax::Camera* camera, const float delta);

}  // namespace requiem::camera_util

#endif  // REQUIEM_UTIL_CAMERA_UTIL_H_
