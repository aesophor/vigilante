// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "CameraUtil.h"

#include "Constants.h"
#include "util/RandUtil.h"

using cocos2d::Director;
using cocos2d::Camera;
using cocos2d::TMXTiledMap;
using cocos2d::Vec2;

namespace {

float duration = 0;
float currentTime = 0;
float power = 0;
float currentPower = 0;
Vec2 pos;

} // namespace


namespace vigilante {

namespace camera_util {

void boundCamera(Camera* camera, GameMap* gameMap) {
  auto winSize = Director::getInstance()->getWinSize();
  Vec2 position = camera->getPosition();

  float mapWidth = gameMap->getWidth();
  float mapHeight = gameMap->getHeight();

  float startX = 0;
  float startY = 0;
  float endX = mapWidth - winSize.width;
  float endY = mapHeight - winSize.height;

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

  // If the map width is smaller than winSize.width,
  // place the map at the center of the window.
  if (mapWidth < winSize.width) {
    position.x = -(winSize.width - mapWidth) / 2;
  }
  if (mapHeight < winSize.height) {
    position.y = -(winSize.height - mapHeight) / 2;
  }

  camera->setPosition(position);
}


void lerpToTarget(Camera* camera, const b2Vec2& target) {
  auto winSize = Director::getInstance()->getWinSize();
  Vec2 position = camera->getPosition();
  position.x = camera->getPositionX() + ((target.x * kPpm - winSize.width / 2) - camera->getPositionX()) * .1f;
  position.y = camera->getPositionY() + ((target.y * kPpm - winSize.height / 2) - camera->getPositionY()) * .1f;
  camera->setPosition(position);
}


void shake(float rumblePower, float rumbleDuration) {
  ::power = rumblePower;
  ::duration = rumbleDuration;
  ::currentTime = 0;
}

void updateShake(Camera* camera, float delta) {
  if (::duration == 0) {
    return;
  }

  if (::currentTime <= ::duration) {
    ::currentPower = ::power * ((::duration - ::currentTime) / ::duration);
    ::pos.x = (rand_util::randFloat() - 0.5f) * 2 * ::currentPower; // camera offset X
    ::pos.y = (rand_util::randFloat() - 0.5f) * 2 * ::currentPower; // camera offset Y
    ::currentTime += delta;
    // Translate camera
    const Vec2& camPos = camera->getPosition();
    camera->setPosition(camPos.x + ::pos.x, camPos.y + ::pos.y);
  } else {
    ::duration = 0;
  }
}

} // namespace camera_util

} // namespace vigilante
