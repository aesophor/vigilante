// Copyright (c) 2023-2025 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_MAP_PATH_FINDER_H_
#define REQUIEM_MAP_PATH_FINDER_H_

#include <optional>

#include <box2d/box2d.h>

#include "map/NavTiledMap.h"

namespace requiem {

class PathFinder {
 public:
  PathFinder() = default;
  PathFinder(const PathFinder&) = delete;
  PathFinder& operator=(const PathFinder&) = delete;
  virtual ~PathFinder() = default;

  virtual std::optional<b2Vec2> findOptimalNextHop(const b2Vec2& srcPos,
                                                   const b2Vec2& destPos,
                                                   const float followDist) = 0;
};

class SimplePathFinder final : public PathFinder {
 public:
  virtual std::optional<b2Vec2> findOptimalNextHop(const b2Vec2& srcPos,
                                                   const b2Vec2& destPos,
                                                   const float followDist) override;
};

class AStarPathFinder final : public PathFinder {
 public:
  AStarPathFinder(const NavTiledMap& navTiledMap);

  virtual std::optional<b2Vec2> findOptimalNextHop(const b2Vec2& srcPos,
                                                   const b2Vec2& destPos,
                                                   const float followDist) override;
 private:
  const NavTiledMap& _navTiledMap;

  
};

}  // namespace requiem

#endif  // REQUIEM_MAP_PATH_FINDER_H_
