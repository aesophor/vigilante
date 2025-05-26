// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "CameraUtil.h"

#include "Constants.h"
#include "util/RandUtil.h"

USING_NS_AX;

namespace requiem::camera_util {

namespace {

float duration = 0;
float currentTime = 0;
float power = 0;
float currentPower = 0;
Vec2 pos{0, 0};

}  // namespace

void boundCamera(Camera* camera, const GameMap* gameMap) {
  const auto winSize = Director::getInstance()->getWinSize();
  Vec2 position = camera->getPosition();

  const float mapWidth = gameMap->getWidth();
  const float mapHeight = gameMap->getHeight();

  const float startX = winSize.width / 2;
  const float startY = winSize.height / 2;
  const float endX = mapWidth - winSize.width / 2;
  const float endY = mapHeight - winSize.height / 2;

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

  // Center the camera in the middle of the game map
  // if the game map's size is smaller than window size
  if (mapWidth < winSize.width) {
    position.x = mapWidth / 2;
  }
  if (mapHeight < winSize.height) {
    position.y = mapHeight / 2;
  }

  camera->setPosition(position);
}

void lerpToTarget(Camera* camera, const b2Vec2& target) {
  auto winSize = Director::getInstance()->getWinSize();
  Vec2 position = camera->getPosition();
  position.x = camera->getPositionX() + ((target.x * kPpm) - camera->getPositionX()) * .1f;
  position.y = camera->getPositionY() + ((target.y * kPpm) - camera->getPositionY()) * .1f;
  camera->setPosition(position);
}

void shake(float rumblePower, float rumbleDuration) {
  power = rumblePower;
  duration = rumbleDuration;
  currentTime = 0;
}

void updateShake(Camera* camera, const float delta) {
  if (duration == 0) {
    return;
  }

  if (currentTime <= duration) {
    currentPower = power * ((duration - currentTime) / duration);
    pos.x = (rand_util::randFloat() - 0.5f) * 2 * currentPower; // camera offset X
    pos.y = (rand_util::randFloat() - 0.5f) * 2 * currentPower; // camera offset Y
    currentTime += delta;
    // Translate camera
    const Vec2& camPos = camera->getPosition();
    camera->setPosition(camPos.x + pos.x, camPos.y + pos.y);
  } else {
    duration = 0;
  }
}

}  // namespace requiem::camera_util
