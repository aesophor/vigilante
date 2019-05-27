#ifndef VIGILANTE_CAMERA_UTIL_H_
#define VIGILANTE_CAMERA_UTIL_H_

#include "cocos2d.h"
#include "Box2D/Box2D.h"

namespace vigilante {

namespace camera_util {

// Camera following a character
void boundCamera(cocos2d::Camera* camera, cocos2d::TMXTiledMap* map);
void lerpToTarget(cocos2d::Camera* camera, const b2Vec2& target);

// Camera shake
void shake(float rumblePower, float rumbleDuration);
void updateShake(cocos2d::Camera* camera, float delta);

} // namespace camera_util

} // namespace vigilante

#endif // VIGILANTE_CAMERA_UTIL_H_
