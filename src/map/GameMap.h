// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_GAME_MAP_H_
#define VIGILANTE_GAME_MAP_H_

#include <unordered_set>
#include <vector>
#include <string>
#include <memory>

#include <cocos2d.h>
#include <Box2D/Box2D.h>
#include "DynamicActor.h"
#include "Interactable.h"
#include "item/Item.h"
#include "util/Logger.h"

namespace vigilante {

class Character;
class Player;

class GameMap {
 public:
  class Portal : public Interactable {
   public:
    Portal(const std::string& targetTmxMapFileName, int targetPortalId, bool willInteractOnContact, b2Body* body);
    virtual ~Portal();

    virtual void onInteract(Character* user) override;  // Interactable
    virtual bool willInteractOnContact() const override;  // Interactable
    virtual void createHintBubbleFx() override;  // Interactable
    virtual void removeHintBubbleFx() override;  // Interactable

    const std::string& getTargetTmxMapFileName() const;
    int getTargetPortalId() const;

   protected:
    std::string _targetTmxMapFileName;  // new (target) .tmx filename
    int _targetPortalId;  // the portal id in the new (target) map
    bool _willInteractOnContact;  // interact with the portal on contact?
    b2Body* _body;
    cocos2d::Sprite* _hintBubbleFxSprite;
  };

  GameMap(b2World* world, const std::string& tmxMapFileName);
  virtual ~GameMap() = default;

  void createObjects();
  void deleteObjects();
  std::unique_ptr<Player> spawnPlayer() const;
  Item* spawnItem(const std::string& itemJson, float x, float y, int amount=1);


  template <typename ReturnType = DynamicActor>
  ReturnType* showDynamicActor(std::shared_ptr<DynamicActor> actor, float x, float y);

  template <typename ReturnType = DynamicActor>
  std::shared_ptr<ReturnType> removeDynamicActor(DynamicActor* actor);


  std::unordered_set<b2Body*>& getTmxTiledMapBodies();
  cocos2d::TMXTiledMap* getTmxTiledMap() const;
  float getWidth() const;
  float getHeight() const;

 private:
  void createRectangles(const std::string& layerName, short categoryBits, bool collidable, float friction);
  void createPolylines(const std::string& layerName, short categoryBits, bool collidable, float friction);

  void spawnPortals();
  void spawnNpcs();
  void spawnChests();

  b2World* _world;
  std::unordered_set<b2Body*> _tmxTiledMapBodies;
  cocos2d::TMXTiledMap* _tmxTiledMap;

  std::unordered_set<std::shared_ptr<DynamicActor>> _dynamicActors;
  std::vector<std::unique_ptr<GameMap::Portal>> _portals;

  friend class GameMapManager;
};



template <typename ReturnType>
ReturnType* GameMap::showDynamicActor(std::shared_ptr<DynamicActor> actor, float x, float y) {
  ReturnType* shownActor = dynamic_cast<ReturnType*>(actor.get());

  std::shared_ptr<DynamicActor> key(std::shared_ptr<DynamicActor>(), actor.get());
  auto it = _dynamicActors.find(key);
  if (it != _dynamicActors.end()) {
    VGLOG(LOG_ERR, "This DynamicActor is already being shown: %p", actor.get());
    return nullptr;
  }

  actor->showOnMap(x, y);
  _dynamicActors.insert(std::move(actor));
  return shownActor;
}


template <typename ReturnType>
std::shared_ptr<ReturnType> GameMap::removeDynamicActor(DynamicActor* actor) {
  std::shared_ptr<ReturnType> removedActor(nullptr);

  std::shared_ptr<DynamicActor> key(std::shared_ptr<DynamicActor>(), actor);
  auto it = _dynamicActors.find(key);
  if (it == _dynamicActors.end()) {
    VGLOG(LOG_ERR, "This DynamicActor has not yet been shown: %p", actor);
    return nullptr;
  }

  removedActor = std::move(std::dynamic_pointer_cast<ReturnType>(*it));
  removedActor->removeFromMap();
  _dynamicActors.erase(it);
  return removedActor;
}

}  // namespace vigilante

#endif  // VIGILANTE_GAME_MAP_H_
