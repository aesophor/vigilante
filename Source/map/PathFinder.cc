// Copyright (c) 2023-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "PathFinder.h"

#include <limits>

#include "scene/GameScene.h"
#include "scene/SceneManager.h"

using namespace std;

namespace vigilante {

optional<b2Vec2> SimplePathFinder::findOptimalNextHop(const b2Vec2& srcPos,
                                                      const b2Vec2& destPos,
                                                      const float followDist) {
  if (destPos.y - srcPos.y < followDist) {
    return std::nullopt;
  }

  const b2Body* closestPlatformBody = nullptr;
  float minDist = numeric_limits<float>::max();

  GameMap* gameMap = SceneManager::the().getCurrentScene<GameScene>()->
      getGameMapManager()->getGameMap();

  for (auto platformBody : gameMap->getTmxTiledMapPlatformBodies()) {
    const b2Vec2& platformPos = platformBody->GetPosition();
    if (platformPos.y < srcPos.y) {
      continue;
    }

    const float dist = std::abs(platformPos.y - srcPos.y);
    if (dist < minDist) {
      closestPlatformBody = platformBody;
      minDist = dist;
    }
  }

  if (!closestPlatformBody) {
    return std::nullopt;
  }

  b2Vec2 targetPos = closestPlatformBody->GetPosition();
  targetPos.y += .2f;
  return targetPos;
}

}  // namespace vigilante
