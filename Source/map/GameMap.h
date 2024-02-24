// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_MAP_GAME_MAP_H_
#define VIGILANTE_MAP_GAME_MAP_H_

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
  friend class GameMapManager;
  friend class GameState;

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

  class Trigger final : public Interactable {
   public:
    Trigger(const std::vector<std::string>& cmds,
            const bool canBeTriggeredOnlyOnce,
            const bool canBeTriggeredOnlyByPlayer,
            const bool shouldBlockWhileInBossFight,
            const std::string& controlHintText,
            const int damage,
            b2Body* body);
    virtual ~Trigger();

    // Executes certain commands via ui/console/Console.cc
    // when the player's body collides with `this->_body`
    virtual void onInteract(Character* user) override;  // Interactable
    virtual bool willInteractOnContact() const override;  // Interactable
    virtual void showHintUI() override;  // Interactable
    virtual void hideHintUI() override;  // Interactable

    void onBossFightBegin();
    void onBossFightEnd();

    inline bool canBeTriggeredOnlyOnce() const { return _canBeTriggeredOnlyOnce; }
    inline bool canBeTriggeredOnlyByPlayer() const { return _canBeTriggeredOnlyByPlayer; }
    inline const std::string& getControlHint() const { return _controlHintText; }
    inline int getDamage() const { return _damage; }
    inline bool hasTriggered() const { return _hasTriggered; }
    inline void setTriggered(bool triggered) { _hasTriggered = triggered; }
    inline b2Body* getBody() const { return _body; }

   protected:
    virtual void createHintBubbleFx() override {}  // Interactable
    virtual void removeHintBubbleFx() override {}  // Interactable

    std::vector<std::string> _cmds;
    bool _canBeTriggeredOnlyOnce{};
    bool _canBeTriggeredOnlyByPlayer{};
    bool _shouldBlockWhileInBossFight{};
    std::string _controlHintText{};
    int _damage{};

    bool _hasTriggered{};
    b2Body* _body{};
  };

  class Portal final : public Interactable {
   public:
    Portal(const std::string& tmxMapFilePath, const int portalId,
           const std::string& targetTmxMapFilePath, const int targetPortalId,
           bool willInteractOnContact, bool isLocked,
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

    //inline const std::string& getTmxMapFilePath() const { return _tmxMapFilePath; }
    //inline int getPortalId() const { return _portalId; }
    inline const std::string& getDestTmxMapFilePath() const { return _destTmxMapFilePath; }
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

    const std::string _tmxMapFilePath;
    const int _portalId;
    const std::string _destTmxMapFilePath;  // new (destination) .tmx filepath
    const int _destPortalId{};  // the portal id in the new (destination) map
    bool _willInteractOnContact{};  // interact with the portal on contact?
    bool _isLocked{};
    b2Body* _body{};
    ax::Sprite* _hintBubbleFxSprite{};
  };

  GameMap(b2World* world, const std::string& tmxMapFilePath);
  ~GameMap();

  void update(const float delta);

  void createObjects();
  std::unique_ptr<Player> createPlayer() const;
  Item* createItem(const std::string& itemJson, float x, float y, int amount=1);

  template <typename ReturnType = StaticActor>
  ReturnType* showStaticActor(std::shared_ptr<StaticActor> actor, float x, float y);

  template <typename ReturnType = StaticActor>
  std::shared_ptr<ReturnType> removeStaticActor(StaticActor* actor);

  template <typename ReturnType = DynamicActor>
  ReturnType* showDynamicActor(std::shared_ptr<DynamicActor> actor, float x, float y);

  template <typename ReturnType = DynamicActor>
  std::shared_ptr<ReturnType> removeDynamicActor(DynamicActor* actor);

  bool onBossFightBegin(const std::string& targetNpcJsonFilePath, const std::string& bgmFilePath);
  void onBossFightEnd();

  inline ax::TMXTiledMap* getTmxTiledMap() const { return _tmxTiledMap; }
  inline const std::string& getTmxTiledMapFilePath() const { return _tmxTiledMapFilePath; }
  inline const std::string& getBgmFilePath() const { return _bgmFilePath; }
  inline const std::string& getLocationName() const { return _locationName; }
  inline PathFinder* getPathFinder() const { return _pathFinder.get(); }
  inline const std::unordered_set<std::shared_ptr<DynamicActor>>& getDynamicActors() const { return _dynamicActors; }
  inline const std::list<b2Body*> getTmxTiledMapPlatformBodies() const { return _tmxTiledMapPlatformBodies; }

  float getWidth() const;
  float getHeight() const;

 private:
  ax::ValueVector getObjects(const std::string& layerName);
  std::list<b2Body*> createRectangles(const std::string& layerName, const short categoryBits,
                                      const bool collidable, const float defaultFriction);
  std::list<b2Body*> createPolylines(const std::string& layerName, const short categoryBits,
                                     const bool collidable, const float defaultFriction);

  void createTriggers();
  void createPortals();
  void createNpcs();
  void createChests();
  void createLightSources();
  void createAnimatedObjects();
  void createParallaxBackground();

  b2World* _world{};
  ax::TMXTiledMap* _tmxTiledMap{};
  std::string _tmxTiledMapFilePath;
  std::string _bgmFilePath;
  std::string _locationName;
  std::list<b2Body*> _tmxTiledMapBodies;
  std::list<b2Body*> _tmxTiledMapPlatformBodies;
  std::unordered_set<std::shared_ptr<StaticActor>> _staticActors;
  std::unordered_set<std::shared_ptr<DynamicActor>> _dynamicActors;
  std::vector<std::unique_ptr<GameMap::Trigger>> _triggers;
  std::vector<std::unique_ptr<GameMap::Portal>> _portals;
  std::unique_ptr<ParallaxBackground> _parallaxBackground;
  std::unique_ptr<PathFinder> _pathFinder;
  bool _isInBossFight{};
};

template <typename ReturnType>
ReturnType* GameMap::showStaticActor(std::shared_ptr<StaticActor> actor, float x, float y) {
  ReturnType* shownActor = dynamic_cast<ReturnType*>(actor.get());

  std::shared_ptr<StaticActor> key{std::shared_ptr<StaticActor>(), actor.get()};
  if (_staticActors.contains(key)) {
    VGLOG(LOG_ERR, "This StaticActor is already being shown: %p", actor.get());
    return nullptr;
  }

  actor->showOnMap(x, y);
  _staticActors.insert(std::move(actor));
  return shownActor;
}

template <typename ReturnType>
std::shared_ptr<ReturnType> GameMap::removeStaticActor(StaticActor* actor) {
  std::shared_ptr<ReturnType> removedActor{nullptr};

  std::shared_ptr<StaticActor> key{std::shared_ptr<StaticActor>{}, actor};
  auto it = _staticActors.find(key);
  if (it == _staticActors.end()) {
    VGLOG(LOG_ERR, "This StaticActor has not yet been shown: %p", actor);
    return nullptr;
  }

  removedActor = std::move(std::dynamic_pointer_cast<ReturnType>(*it));
  removedActor->removeFromMap();
  _staticActors.erase(it);
  return removedActor;
}

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

#endif  // VIGILANTE_MAP_GAME_MAP_H_
