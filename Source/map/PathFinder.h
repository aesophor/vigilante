// Copyright (c) 2023-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_MAP_PATH_FINDER_H_
#define VIGILANTE_MAP_PATH_FINDER_H_

#include <optional>

#include <box2d/box2d.h>

namespace vigilante {

class PathFinder {
 public:
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

}  // namespace vigilante

#endif  // VIGILANTE_MAP_PATH_FINDER_H_
