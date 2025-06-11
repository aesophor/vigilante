// Copyright (c) 2025 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_MAP_NAV_TILED_MAP_H_
#define REQUIEM_MAP_NAV_TILED_MAP_H_

#include <vector>

#include <axmol.h>

namespace requiem {

class NavTiledMap final {
 public:
  enum class TileType {
    GROUND_WALL,
    PLATFORM,
    TRAP,
    EMPTY
  };

  struct NavTile {
    TileType type{TileType::EMPTY};
    bool hasSurfaceBelow{};
    bool hasTrapBelow{};
    bool canJumpDown{};
  };

  explicit NavTiledMap(const ax::TMXTiledMap& tmxTiledMap);
  NavTiledMap(const NavTiledMap&) = delete;
  NavTiledMap& operator=(const NavTiledMap&) = delete;

  ax::Vec2 getTileCoordinate(const ax::Vec2& pos) const;
  ax::Vec2 getTilePos(const ax::Vec2& tileCoordinate) const;
  const NavTile& getNavTile(const ax::Vec2& tileCoordinate) const;

  inline const ax::TMXTiledMap& getTmxTiledMap() const { return _tmxTiledMap; }
  inline ax::FastTMXLayer* getBitmapLayer() const { return _bitmapLayer; }

 private:
  std::vector<std::vector<NavTile>> buildNavTiles() const;
  TileType getTileType(const ax::Vec2& tileCoordinate) const;

  const ax::TMXTiledMap& _tmxTiledMap;
  ax::FastTMXLayer* _bitmapLayer{};
  
  std::vector<std::vector<NavTile>> _navTiles;
};

}  // namespace requiem

#endif  // REQUIEM_MAP_NAV_TILED_MAP_H_
