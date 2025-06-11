// Copyright (c) 2025 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "NavTiledMap.h"

using namespace std;
USING_NS_AX;

namespace requiem {

NavTiledMap::NavTiledMap(const TMXTiledMap& tmxTiledMap)
    : _tmxTiledMap{tmxTiledMap},
      _bitmapLayer{tmxTiledMap.getLayer("Bitmap")},
      _navTiles{buildNavTiles()} {}

ax::Vec2 NavTiledMap::getTileCoordinate(const ax::Vec2& pos) const {
  const float tileWidth = _tmxTiledMap.getTileSize().width;
  const float tileHeight = _tmxTiledMap.getTileSize().height;
  const int mapHeight = _tmxTiledMap.getMapSize().height;

  const int tileX = static_cast<int>(pos.x / tileWidth);
  const int tileY = mapHeight - 1 - static_cast<int>(pos.y / tileHeight);
  return Vec2(tileX, tileY);
}

ax::Vec2 NavTiledMap::getTilePos(const ax::Vec2& tileCoordinate) const {
  const float tileWidth = _tmxTiledMap.getTileSize().width;
  const float tileHeight = _tmxTiledMap.getTileSize().height;
  const int mapHeight = _tmxTiledMap.getMapSize().height;

  const int posX = (tileCoordinate.x + 0.5f) * tileWidth;
  const int posY = (mapHeight - 1 - tileCoordinate.y + 0.5f) * tileHeight;
  return Vec2(posX, posY);
}

const NavTiledMap::NavTile& NavTiledMap::getNavTile(const ax::Vec2 &tileCoordinate) const {
  return _navTiles[tileCoordinate.x][tileCoordinate.y];
}

vector<vector<NavTiledMap::NavTile>> NavTiledMap::buildNavTiles() const {
  if (!_bitmapLayer) {
    return {};
  }

  // Make navTiles column-major so that the way we access its individual cell
  // would appear more natural.
  const int numCols = _tmxTiledMap.getMapSize().x;
  const int numRows = _tmxTiledMap.getMapSize().y;
  vector<vector<NavTile>> navTiles(numCols, vector<NavTile>(numRows));

  for (int x = 0; x < numCols; x++) {
    for (int y = 0; y < numRows; y++) {
      navTiles[x][y].type = getTileType(Vec2(x, y));
    }
  }

  for (int x = 0; x < numCols; x++) {
    for (int y = 0; y < numRows; y++) {
      const NavTile& tile = navTiles[x][y];
      if (tile.type == TileType::GROUND_WALL || tile.type == TileType::PLATFORM) {
        for (int i = y - 1; i >= 0; i--) {
          if (navTiles[x][i].type != TileType::EMPTY) {
            break;
          }
          navTiles[x][i].hasSurfaceBelow = true;
        }
      }
    }
  }

  for (int x = 0; x < numCols; x++) {
    for (int y = 0; y < numRows; y++) {
      const NavTile& tile = navTiles[x][y];
      if (tile.type == TileType::TRAP) {
        for (int i = y - 1; i >= 0; i--) {
          if (navTiles[x][i].type != TileType::EMPTY) {
            break;
          }
          navTiles[x][i].hasTrapBelow = true;
        }
      }
    }
  }

  for (int x = 0; x < numCols; x++) {
    for (int y = 0; y < numRows; y++) {
      const NavTile& tile = navTiles[x][y];
      if (tile.type == TileType::TRAP) {
        for (int i = y - 1; i >= 0; i--) {
          if (navTiles[x][i].type != TileType::EMPTY) {
            break;
          }
          navTiles[x][i].hasTrapBelow = true;
        }
      }
    }
  }

  return navTiles;
}

NavTiledMap::TileType NavTiledMap::getTileType(const ax::Vec2& tileCoordinate) const {
  const int gid = _bitmapLayer->getTileGIDAt(tileCoordinate);
  if (!gid) {
    return TileType::EMPTY;
  }

  const int baseGid = _bitmapLayer->getTileSet()->_firstGid;
  return static_cast<TileType>(gid - baseGid);
}

}  // namespace requiem
