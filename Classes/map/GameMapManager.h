#ifndef VIGILANTE_GAMEMAP_MANAGER_H_
#define VIGILANTE_GAMEMAP_MANAGER_H_

#include <set>
#include <string>
#include <memory>

#include "cocos2d.h"
#include "Box2D/Box2D.h"

#include "character/Character.h"
#include "item/Item.h"
#include "map/WorldContactListener.h"

namespace vigilante {

class Player;

class GameMapManager : public cocos2d::Layer {
 public:
  static GameMapManager* getInstance();
  virtual bool init() override;
  virtual ~GameMapManager() = default;

  void load(const std::string& mapFileName);
  void createDustFx(Character* character);

  b2World* getWorld() const;
  cocos2d::TMXTiledMap* getMap() const;
  Player* getPlayer() const;

  std::set<std::unique_ptr<Character>>& getCharacters();
  std::set<Item*>& getItems();

 private:
  static GameMapManager* _instance;
  GameMapManager() = default;

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

  std::set<std::unique_ptr<Character>> _characters;
  std::set<Item*> _items;

  cocos2d::TMXTiledMap* _map;
  Player* _player;
};

} // namespace vigilante

#endif // VIGILANTE_GAMEMAP_MANAGER_H_
