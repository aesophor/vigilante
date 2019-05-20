#ifndef VIGILANTE_GAME_MAP_H_
#define VIGILANTE_GAME_MAP_H_

#include <unordered_set>
#include <vector>
#include <string>
#include <memory>

#include "cocos2d.h"
#include "Box2D/Box2D.h"

namespace vigilante {

class Character;
class Item;
class Player;

class GameMap {
 public:
  friend class GameMapManager;

  struct Portal {
    Portal(const std::string& targetTmxMapFileName, int targetPortalId, b2Body* body);
    std::string targetTmxMapFileName;
    int targetPortalId;
    b2Body* body;
  };

  GameMap(b2World* world, const std::string& tmxMapFileName);
  virtual ~GameMap();

  std::unordered_set<b2Body*>& getTmxTiledMapBodies();
  cocos2d::TMXTiledMap* getTmxTiledMap() const;

  std::unordered_set<std::unique_ptr<Character>>& getNpcs();
  std::unordered_set<std::unique_ptr<Item>>& getDroppedItems();
  const std::vector<std::unique_ptr<GameMap::Portal>>& getPortals() const;

 private:
  void createRectangles(const std::string& layerName, short categoryBits, bool collidable, float friction);
  void createPolylines(const std::string& layerName, short categoryBits, bool collidable, float friction);
  void createPortals();
  Player* createPlayer() const;

  b2World* _world;
  std::unordered_set<b2Body*> _tmxTiledMapBodies;
  cocos2d::TMXTiledMap* _tmxTiledMap;

  std::unordered_set<std::unique_ptr<Character>> _npcs;
  std::unordered_set<std::unique_ptr<Item>> _droppedItems;
  std::vector<std::unique_ptr<GameMap::Portal>> _portals;
};

} // namespace vigilante

#endif // VIGILANTE_GAME_MAP_H_
