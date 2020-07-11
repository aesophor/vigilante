// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_GAME_MAP_H_
#define VIGILANTE_GAME_MAP_H_

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <memory>

#include <cocos2d.h>
#include <Box2D/Box2D.h>
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

   protected:
    std::string _targetTmxMapFileName; // new (target) .tmx filename
    int _targetPortalId; // the portal id in the new (target) map
    bool _willInteractOnContact; // interact with the portal on contact?
    b2Body* _body;
  };

  GameMap(b2World* world, const std::string& tmxMapFileName);
  virtual ~GameMap() = default;

  void createObjects();
  void deleteObjects();

  std::unordered_set<b2Body*>& getTmxTiledMapBodies();
  cocos2d::TMXTiledMap* getTmxTiledMap() const;

  // Add the given DynamicActor to this map. The actor's lifetime
  // will be managed be this map.
  void addDynamicActor(DynamicActor* actor);

  // Remove the specified DynamicActor from this map. unique_ptr::release()
  // will be called for the underlying unique_ptr. The callee must find
  // a way to manage the removed actor's lifetime.
  void removeDynamicActor(const DynamicActor* actor);

  Player* createPlayer() const;
  Item* spawnItem(const std::string& itemJson, float x, float y, int amount=1);

  float getWidth() const;
  float getHeight() const;

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

  std::unordered_map<const DynamicActor*, std::unique_ptr<DynamicActor>> _dynamicActors;
  std::vector<std::unique_ptr<GameMap::Portal>> _portals;

  friend class GameMapManager;
};

} // namespace vigilante

#endif // VIGILANTE_GAME_MAP_H_
