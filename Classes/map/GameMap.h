#ifndef VIGILANTE_GAME_MAP_H_
#define VIGILANTE_GAME_MAP_H_

#include "cocos2d.h"
#include "Box2D/Box2D.h"

namespace vigilante {

class GameMap {
 public:
  GameMap(b2World* world);
  virtual ~GameMap();

 private:
  cocos2d::TMXTiledMap _map;
  int _width;
  int _height;
  int _tileSize;
};

} // namespace vigilante

#endif // VIGILANTE_GAME_MAP_H_
