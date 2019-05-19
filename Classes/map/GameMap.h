#ifndef VIGILANTE_GAME_MAP_H_
#define VIGILANTE_GAME_MAP_H_

#include <unordered_set>
#include <unordered_map>
#include <string>

#include "cocos2d.h"
#include "Box2D/Box2D.h"

namespace vigilante {

class Character;
class Item;
class Player;
class Portal;

class GameMap {
 public:
  friend class GameMapManager;
  GameMap(b2World* world, const std::string& tmxMapFileName);
  virtual ~GameMap();

  std::unordered_set<b2Body*>& getTmxTiledMapBodies();
  cocos2d::TMXTiledMap* getTmxTiledMap() const;

  std::unordered_set<Character*>& getNpcs();
  std::unordered_set<Item*>& getDroppedItems();
  const std::unordered_map<b2Body*, Portal*>& getPortals() const;

 private:
  void createRectangles(const std::string& layerName, short categoryBits, bool collidable, float friction);
  void createPolylines(const std::string& layerName, short categoryBits, bool collidable, float friction);
  void createPortals();
  Player* createPlayer() const;

  b2World* _world;
  std::unordered_set<b2Body*> _tmxTiledMapBodies;
  cocos2d::TMXTiledMap* _tmxTiledMap;

  std::unordered_set<Character*> _npcs;
  std::unordered_set<Item*> _droppedItems;
  std::unordered_map<b2Body*, Portal*> _portals;
};

} // namespace vigilante

#endif // VIGILANTE_GAME_MAP_H_
