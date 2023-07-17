// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_GAME_MAP_H_
#define VIGILANTE_GAME_MAP_H_

#include <list>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include <axmol.h>

#include <box2d/box2d.h>

#include "DynamicActor.h"
#include "Interactable.h"
#include "item/Item.h"
#include "map/ParallaxBackground.h"
#include "map/PathFinder.h"
#include "util/Logger.h"

namespace vigilante {

class Character;
class Player;

class GameMap final {
 public:
  enum class PhysicalLayer {
    GROUND,
    WALL,
    PLATFORM,
    PivotMarker,
    CliffMarker,
    SIZE
  };

  enum class OpenableObjectType {
    PORTAL,
    CHEST,
    SIZE
  };

  class Trigger : public Interactable {
   public:
    Trigger(const std::vector<std::string>& cmds,
            const bool canBeTriggeredOnlyOnce,
            const bool canBeTriggeredOnlyByPlayer,
            b2Body* body);
    virtual ~Trigger();

    // Executes certain commands via ui/console/Console.cc
    // when the player's body collides with `this->_body`
    virtual void onInteract(Character* user) override;  // Interactable
    virtual bool willInteractOnContact() const override { return true; }  // Interactable
    virtual void showHintUI() override {}  // Interactable
    virtual void hideHintUI() override {}  // Interactable

    inline bool canBeTriggeredOnlyOnce() const { return _canBeTriggeredOnlyOnce; }
    inline bool canBeTriggeredOnlyByPlayer() const { return _canBeTriggeredOnlyByPlayer; }
    inline bool hasTriggered() const { return _hasTriggered; }
    inline void setTriggered(bool triggered) { _hasTriggered = triggered; }

   protected:
    virtual void createHintBubbleFx() override {}  // Interactable
    virtual void removeHintBubbleFx() override {}  // Interactable

    std::vector<std::string> _cmds;
    bool _canBeTriggeredOnlyOnce{};
    bool _canBeTriggeredOnlyByPlayer{};
    bool _hasTriggered{};
    b2Body* _body{};
  };

  class Portal : public Interactable {
   public:
    Portal(const std::string& targetTmxMapFileName,
           int targetPortalId,
           bool willInteractOnContact,
           bool isLocked,
           b2Body* body);
    virtual ~Portal();

    virtual void onInteract(Character* user) override;  // Interactable
    virtual bool willInteractOnContact() const override;  // Interactable
    virtual void showHintUI() override;  // Interactable
    virtual void hideHintUI() override;  // Interactable

    bool canBeUnlockedBy(Character* user) const;
    inline bool isLocked() const { return _isLocked; }
    void lock();
    void unlock();

    inline const std::string& getDestTmxMapFileName() const { return _destTmxMapFileName; }
    inline int getDestPortalId() const { return _destPortalId; }

   protected:
    virtual void createHintBubbleFx() override;  // Interactable
    virtual void removeHintBubbleFx() override;  // Interactable

    // Try to unlock the portal as `user`.
    // If the portal is already unlocked, then this is a no-op.
    void maybeUnlockPortalAs(Character* user);

    // Save the current portal's lock/unlock state in `_allPortalStates`.
    void saveLockUnlockState() const;
    int getPortalId() const;

    std::string _destTmxMapFileName;  // new (destination) .tmx filename
    int _destPortalId{};  // the portal id in the new (destination) map
    bool _willInteractOnContact{};  // interact with the portal on contact?
    bool _isLocked{};
    b2Body* _body{};
    ax::Sprite* _hintBubbleFxSprite{};
  };

  GameMap(b2World* world, const std::string& tmxMapFileName);
  ~GameMap();

  void update(float delta);
  
  void createObjects();
  std::unique_ptr<Player> createPlayer() const;
  Item* createItem(const std::string& itemJson, float x, float y, int amount=1);

  template <typename ReturnType = DynamicActor>
  ReturnType* showDynamicActor(std::shared_ptr<DynamicActor> actor, float x, float y);

  template <typename ReturnType = DynamicActor>
  std::shared_ptr<ReturnType> removeDynamicActor(DynamicActor* actor);

  inline ax::TMXTiledMap* getTmxTiledMap() const { return _tmxTiledMap; }
  inline const std::string& getTmxTiledMapFileName() const { return _tmxTiledMapFileName; }
  inline const std::string& getBgmFileName() const { return _bgmFileName; }
  inline PathFinder* getPathFinder() const { return _pathFinder.get(); }
  inline const std::list<b2Body*>
  getTmxTiledMapPlatformBodies() const { return _tmxTiledMapPlatformBodies; }

  float getWidth() const;
  float getHeight() const;

 private:
  ax::ValueVector getObjects(const std::string& layerName);
  std::list<b2Body*> createRectangles(const std::string& layerName, short categoryBits,
                                      bool collidable, float friction);
  std::list<b2Body*> createPolylines(const std::string& layerName, short categoryBits,
                                     bool collidable, float friction);

  void createTriggers();
  void createPortals();
  void createNpcs();
  void createChests();
  void createParallaxBackground();

  b2World* _world{};
  ax::TMXTiledMap* _tmxTiledMap{};
  std::string _tmxTiledMapFileName;
  std::string _bgmFileName;
  std::list<b2Body*> _tmxTiledMapBodies;
  std::list<b2Body*> _tmxTiledMapPlatformBodies;
  std::unordered_set<std::shared_ptr<DynamicActor>> _dynamicActors;
  std::vector<std::unique_ptr<GameMap::Trigger>> _triggers;
  std::vector<std::unique_ptr<GameMap::Portal>> _portals;
  std::unique_ptr<ParallaxBackground> _parallaxBackground;
  std::unique_ptr<PathFinder> _pathFinder;

  friend class GameMapManager;
  friend class GameState;
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
