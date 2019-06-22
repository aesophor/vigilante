#ifndef VIGILANTE_GAME_MAP_H_
#define VIGILANTE_GAME_MAP_H_

#include <unordered_set>
#include <vector>
#include <string>
#include <memory>

#include "cocos2d.h"
#include "Box2D/Box2D.h"

#include "DynamicActor.h"
#include "Interactable.h"
#include "item/Item.h"

namespace vigilante {

class Character;
class Player;

class GameMap {
 public:
  class Portal : public Interactable {
   public:
    Portal(const std::string& targetTmxMapFileName, int targetPortalId, bool willInteractOnContact, b2Body* body);
    virtual ~Portal();
    virtual void onInteract(Character* user) override; // Interactable
    virtual bool willInteractOnContact() const override; // Interactable

    const std::string& getTargetTmxMapFileName() const;
    int getTargetPortalId() const;
    b2Body* getBody() const;

   protected:
    std::string _targetTmxMapFileName; // new (target) .tmx filename
    int _targetPortalId; // the portal id in the new (target) map
    bool _willInteractOnContact; // interact with the portal on contact?
    b2Body* _body;
  };

  GameMap(b2World* world, const std::string& tmxMapFileName);
  virtual ~GameMap();

  std::unordered_set<b2Body*>& getTmxTiledMapBodies();
  cocos2d::TMXTiledMap* getTmxTiledMap() const;

  std::unordered_set<DynamicActor*>& getDynamicActors();
  const std::vector<GameMap::Portal*>& getPortals() const;

  Player* createPlayer() const;
  Item* spawnItem(const std::string& itemJson, float x, float y, int amount=1);

 private:
  void createRectangles(const std::string& layerName, short categoryBits, bool collidable, float friction);
  void createPolylines(const std::string& layerName, short categoryBits, bool collidable, float friction);
  void createPortals();
  void createNpcs();
  void createEnemies();
  void createChests();

  b2World* _world;
  std::unordered_set<b2Body*> _tmxTiledMapBodies;
  cocos2d::TMXTiledMap* _tmxTiledMap;

  std::unordered_set<DynamicActor*> _dynamicActors;
  std::vector<GameMap::Portal*> _portals;
};

} // namespace vigilante

#endif // VIGILANTE_GAME_MAP_H_
