#ifndef VIGILANTE_GAMEMAP_MANAGER_H_
#define VIGILANTE_GAMEMAP_MANAGER_H_

#include <string>
#include <vector>
#include <memory>

#include "cocos2d.h"
#include "Box2D/Box2D.h"

#include "character/Player.h"
#include "character/Enemy.h"
#include "map/WorldContactListener.h"

namespace vigilante {

class Player;

class GameMapManager {
 public:
  static GameMapManager* getInstance();
  virtual ~GameMapManager();
  void load(const std::string& mapFileName);

  b2World* getWorld() const;

  cocos2d::Layer* getLayer() const;
  cocos2d::TMXTiledMap* getMap() const;
  Player* getPlayer() const;
  std::vector<Character*> getCharacters() const;

 private:
  static GameMapManager* _instance;
  GameMapManager(const b2Vec2& gravity);

  Player* spawnPlayer();

  static void createRectangles(b2World* world,
                               cocos2d::TMXTiledMap* map,
                               const std::string& layerName,
                               short categoryBits,
                               bool isCollidable,
                               float friction);

  static void createPolylines(b2World* world,
                              cocos2d::TMXTiledMap* map,
                              const std::string& layerName,
                              short categoryBits,
                              bool isCollidable,
                              float friction);

  std::unique_ptr<b2World> _world;
  std::unique_ptr<WorldContactListener> _worldContactListener;

  cocos2d::Layer* _layer;
  cocos2d::TMXTiledMap* _map;
  Player* _player;
  std::vector<Character*> _characters;
};

} // namespace vigilante

#endif // VIGILANTE_GAMEMAP_MANAGER_H_
