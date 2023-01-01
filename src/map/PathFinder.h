// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_PATH_FINDER_H_
#define VIGILANTE_PATH_FINDER_H_

#include <optional>

#include <Box2D/Box2D.h>

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

#endif  // VIGILANTE_PATH_FINDER_H_
