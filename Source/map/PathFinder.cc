// Copyright (c) 2023-2025 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "PathFinder.h"

#include <limits>
#include <memory>
#include <queue>
#include <unordered_set>
#include <vector>

#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "util/Logger.h"

using namespace std;
USING_NS_AX;

namespace requiem {

optional<b2Vec2> AStarPathFinder::getNextWaypoint(const b2Vec2& srcPos,
                                                  const b2Vec2& dstPos,
                                                  const float followDist) {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  _navTiledMap = &gmMgr->getGameMap()->getNavTiledMap();

  const Vec2 src = _navTiledMap->getTileCoordinate(Vec2{srcPos.x * kPpm, srcPos.y * kPpm});
  const Vec2 dst = _navTiledMap->getTileCoordinate(Vec2{dstPos.x * kPpm, dstPos.y * kPpm});

  if (_destPos != dst) {
    _path = findPath(src, dst);
    _destPos = dst;
  }
  if (_path.empty()) {
    return nullopt;
  }
  const b2Vec2 ret = _path.front();
  _path.pop_front();
  return ret;
}

void AStarPathFinder::reset() {
  _nodes.clear();
}

list<b2Vec2> AStarPathFinder::findPath(const Vec2& srcTileCoordinate,
                                       const Vec2& dstTileCoordinate) {
  reset();

  auto minHeapCmp = [](const Node* n1, const Node* n2) { return n1->f > n2->f; };
  using MinHeap = priority_queue<Node*, vector<Node*>, decltype(minHeapCmp)>;
  MinHeap frontier{minHeapCmp};
  unordered_set<Node*> openSet;
  unordered_set<Vec2> closedSet;

  Node* root = getOrCreateNode(srcTileCoordinate);
  assert(root);
  frontier.push(root);
  openSet.insert(root);

  while (frontier.size()) {
    Node* current = frontier.top();
    frontier.pop();
    openSet.erase(current);

    if (current->navTileCoordinate == dstTileCoordinate) {
      return reconstructPath(current);
    }

    closedSet.insert(current->navTileCoordinate);

    for (auto neighbor : getNeighbors(current)) {
      if (closedSet.contains(neighbor->navTileCoordinate)) {
        continue;
      }

      const float gTentative = current->g + getCostBetween(current, neighbor);
      if (!openSet.contains(neighbor) || gTentative < neighbor->g) {
        neighbor->parent = current;
        neighbor->g = gTentative;
        neighbor->h = heuristic(neighbor->navTileCoordinate, dstTileCoordinate);
        neighbor->f = neighbor->g + neighbor->h;

        if (!openSet.contains(neighbor)) {
          openSet.insert(neighbor);
          frontier.push(neighbor);
        }
      }
    }
  }

  reset();
  return {};
}

list<b2Vec2> AStarPathFinder::reconstructPath(const Node* dstNode) const {
  list<b2Vec2> path;

  for (const Node* node = dstNode; node; node = node->parent) {
    const Vec2 pos = _navTiledMap->getTilePos(node->navTileCoordinate);
    path.push_front({pos.x / kPpm, pos.y / kPpm});
  }

  return path;
}

list<AStarPathFinder::Node*>AStarPathFinder::getNeighbors(const Node* node) {
  if (!node) {
    return {};
  }

  list<Node*> neighbors;
  const Vec2& mapSize = _navTiledMap->getTmxTiledMap().getMapSize();
  const Vec2& currentPos = node->navTileCoordinate;
  const NavTiledMap::NavTile& currentTile = _navTiledMap->getNavTile(currentPos);

  // move left
  if (currentPos.x > 0) {
     const NavTiledMap::NavTile& leftTile = _navTiledMap->getNavTile({currentPos.x - 1, currentPos.y});
     if (!leftTile.hasTrapBelow) {
       neighbors.push_back(getOrCreateNode({currentPos.x - 1, currentPos.y}));
     }
  }

  // move right
  if (currentPos.x < mapSize.x - 1) {
    const NavTiledMap::NavTile& rightTile = _navTiledMap->getNavTile({currentPos.x + 1, currentPos.y});
    if (!rightTile.hasTrapBelow) {
      neighbors.push_back(getOrCreateNode({currentPos.x + 1, currentPos.y}));
    }
  }

  // jump straight up
  if (currentPos.y > 0) {
    const NavTiledMap::NavTile& aboveTile = _navTiledMap->getNavTile({currentPos.x, currentPos.y - 1});
    neighbors.push_back(getOrCreateNode({currentPos.x, currentPos.y - 1}));

    // jump leftward
    if (currentPos.x > 0) {
      const NavTiledMap::NavTile& jumpRightwardTile = _navTiledMap->getNavTile({currentPos.x - 1, currentPos.y - 1});
      neighbors.push_back(getOrCreateNode({currentPos.x - 1, currentPos.y - 1}));
    }

    // jump rightward
    if (currentPos.x < mapSize.x - 1) {
      const NavTiledMap::NavTile& jumpRightwardTile = _navTiledMap->getNavTile({currentPos.x + 1, currentPos.y - 1});
      neighbors.push_back(getOrCreateNode({currentPos.x + 1, currentPos.y - 1}));
    }
  }

  // move down
  if (currentPos.y < mapSize.y - 1) {
     const NavTiledMap::NavTile& belowTile = _navTiledMap->getNavTile({currentPos.x, currentPos.y + 1});
     if (currentTile.canJumpDown && belowTile.hasSurfaceBelow) {
       neighbors.push_back(getOrCreateNode({currentPos.x, currentPos.y + 1}));
     }
  }

  return neighbors;
}

int AStarPathFinder::getCostBetween(const Node* n1, const Node* n2) const {
  return heuristic(n1->navTileCoordinate, n2->navTileCoordinate);
}

int AStarPathFinder::heuristic(const ax::Vec2& tileCoordinate, const ax::Vec2& dstTileCoordinate) const {
  const int dx = std::abs(dstTileCoordinate.x - tileCoordinate.x);
  const int dy = std::abs(dstTileCoordinate.y - tileCoordinate.y);
  return dx + dy * 3.f;  // punish vertical movement
}

AStarPathFinder::Node* AStarPathFinder::getOrCreateNode(const Vec2 &tileCoordinate) {
  auto it = _nodes.find(tileCoordinate);
  if (it != _nodes.end()) {
    return it->second.get();
  }

  auto newNode = make_unique<Node>(tileCoordinate, nullptr, 0, 0, 0);
  auto newNodeRawPtr = newNode.get();
  _nodes[tileCoordinate] = std::move(newNode);
  return newNodeRawPtr;
}

optional<b2Vec2> SimplePathFinder::getNextWaypoint(const b2Vec2& srcPos,
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

}  // namespace requiem
