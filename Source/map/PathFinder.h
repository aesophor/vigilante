// Copyright (c) 2023-2025 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_MAP_PATH_FINDER_H_
#define REQUIEM_MAP_PATH_FINDER_H_

#include <list>
#include <memory>
#include <optional>
#include <unordered_map>

#include <box2d/box2d.h>

#include "map/NavTiledMap.h"
#include "util/AxUtil.h"

namespace requiem {

class PathFinder {
 public:
  PathFinder() = default;
  PathFinder(const PathFinder&) = delete;
  PathFinder& operator=(const PathFinder&) = delete;
  virtual ~PathFinder() = default;

  virtual std::optional<b2Vec2> getNextWaypoint(const b2Vec2& srcPos,
                                                const b2Vec2& destPos,
                                                const float followDist) = 0;
};

class AStarPathFinder final : public PathFinder {
 public:
  struct Node {
    ax::Vec2 navTileCoordinate{0, 0};
    Node* parent{};
    int g{};  // distance from starting node.
    int h{};  // distance from end node. Aka the heuristic.
    int f{};  // f = g + h
  };

  virtual std::optional<b2Vec2> getNextWaypoint(const b2Vec2& srcPos,
                                                const b2Vec2& dstPos,
                                                const float followDist) override;
  void reset();
  
 private:
  std::list<b2Vec2> findPath(const ax::Vec2& srcTileCoordinate,
                             const ax::Vec2& dstTileCoordinate);
  std::list<b2Vec2> reconstructPath(const Node* dstNode) const;
  std::list<Node*> getNeighbors(const Node* node);
  int getCostBetween(const Node* n1, const Node* n2) const;
  int heuristic(const ax::Vec2& tileCoordinate, const ax::Vec2& dstTileCoordinate) const;
  Node* getOrCreateNode(const ax::Vec2& tileCoordinate);

  const NavTiledMap* _navTiledMap{};
  ax::Vec2 _destPos;
  std::list<b2Vec2> _path;
  std::unordered_map<ax::Vec2, std::unique_ptr<Node>> _nodes;
};

class SimplePathFinder final : public PathFinder {
 public:
  virtual std::optional<b2Vec2> getNextWaypoint(const b2Vec2& srcPos,
                                                const b2Vec2& destPos,
                                                const float followDist) override;
};

}  // namespace requiem

#endif  // REQUIEM_MAP_PATH_FINDER_H_
