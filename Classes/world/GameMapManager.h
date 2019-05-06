#ifndef VIGILANTE_GAMEMAP_MANAGER_H_
#define VIGILANTE_GAMEMAP_MANAGER_H_

#include <string>

#include "cocos2d.h"
#include "Box2D/Box2D.h"

namespace vigilante {

class GameMapManager {
 public:
  GameMapManager(b2World* world);
  virtual ~GameMapManager();
  void load(const std::string& mapFileName);

  b2World* getWorld() const;
  cocos2d::TMXTiledMap* getMap() const;

 private:
  b2World* _world;  
  cocos2d::TMXTiledMap* _map;
};

} // namespace vigilante

#endif // VIGILANTE_GAMEMAP_MANAGER_H_
