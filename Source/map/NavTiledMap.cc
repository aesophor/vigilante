// Copyright (c) 2025 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "NavTiledMap.h"

using namespace std;
USING_NS_AX;

namespace requiem {

NavTiledMap::NavTiledMap(const TMXTiledMap& tmxTiledMap)
    : _tmxTiledMap{tmxTiledMap},
      _bitmapLayer{tmxTiledMap.getLayer("Bitmap")} {
  if (!_bitmapLayer) {
    return;
  }

  const int numRows = _tmxTiledMap.getMapSize().y;
  const int numCols = _tmxTiledMap.getMapSize().x;
  _navTiles.resize(numRows, vector<NavTile>(numCols));

  for (int y = 0; y < numRows; y++) {
    for (int x = 0; x < numCols; x++) {
      _navTiles[y][x].type = getTileType(Vec2(x, y));
    }
  }

  for (int y = 0; y < numRows; y++) {
    for (int x = 0; x < numCols; x++) {
      const NavTile& tile = _navTiles[y][x];
      if (tile.type == TileType::GROUND_WALL || tile.type == TileType::PLATFORM) {
        for (int i = y + 1; i < numRows; i++) {
          if (_navTiles[i][x].type != TileType::EMPTY) {
            break;
          }
          _navTiles[i][x].hasSurfaceBelow = true;
        }
      }
    }
  }

  for (int y = 0; y < numRows; y++) {
    for (int x = 0; x < numCols; x++) {
      const NavTile& tile = _navTiles[y][x];
      if (tile.type == TileType::TRAP) {
        for (int i = y + 1; i < numRows; i++) {
          if (_navTiles[i][x].type != TileType::EMPTY) {
            break;
          }
          _navTiles[i][x].hasTrapBelow = true;
        }
      }
    }
  }

  for (int y = 0; y < numRows; y++) {
    for (int x = 0; x < numCols; x++) {
      const NavTile& tile = _navTiles[y][x];
      if (tile.type == TileType::TRAP) {
        for (int i = y + 1; i < numRows; i++) {
          if (_navTiles[i][x].type != TileType::EMPTY) {
            break;
          }
          _navTiles[i][x].hasTrapBelow = true;
        }
      }
    }
  }
}

NavTiledMap::TileType NavTiledMap::getTileType(const ax::Vec2& tileCoordinate) const {
  const int gid = _bitmapLayer->getTileGIDAt(tileCoordinate);
  if (!gid) {
    return TileType::EMPTY;
  }

  const int baseGid = _bitmapLayer->getTileSet()->_firstGid;
  return static_cast<TileType>(gid - baseGid);
}

Vec2 NavTiledMap::getTileCoordinate(const ax::Vec2& pos) const {
  return {
    pos.x / _tmxTiledMap.getTileSize().x,
    pos.y / _tmxTiledMap.getTileSize().y
  };
}

}  // namespace requiem
