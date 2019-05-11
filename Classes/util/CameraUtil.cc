#include "CameraUtil.h"

#include "util/Constants.h"

using cocos2d::Camera;
using cocos2d::TMXTiledMap;
using cocos2d::Vec2;


namespace vigilante {

namespace camera_util {

void boundCamera(Camera* camera, TMXTiledMap* map) {
  Vec2 position = camera->getPosition();
  float tileSize = map->getTileSize().width;

  float startX = 0;
  float startY = 0;
  float endX = (map->getMapSize().width * tileSize) - 600;
  float endY = (map->getMapSize().height * tileSize) - 300;

  if (position.x < startX) {
    position.x = startX;
  }
  if (position.y < startY) {
    position.y = startY;
  }

  if (position.x > endX) {
    position.x = endX;
  }
  if (position.y > endY) {
    position.y = endY;
  }

  camera->setPosition(position);
}

void lerpToTarget(Camera* camera, const b2Vec2& target) {
  Vec2 position = camera->getPosition();
  position.x = camera->getPositionX() + ((target.x * kPpm - 600 / 2) - camera->getPositionX()) * .1f;
  position.y = camera->getPositionY() + ((target.y * kPpm - 300 / 2) - camera->getPositionY()) * .1f;
  camera->setPosition(position);
}

} // namespace camera_util

} // namespace vigilante
