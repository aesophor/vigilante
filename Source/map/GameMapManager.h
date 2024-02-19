// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_MAP_GAME_MAP_MANAGER_H_
#define VIGILANTE_MAP_GAME_MAP_MANAGER_H_

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <unordered_set>

#include <axmol.h>

#include <box2d/box2d.h>

#include "Controllable.h"
#include "character/Character.h"
#include "character/Player.h"
#include "item/Item.h"
#include "map/GameMap.h"
#include "map/WorldContactListener.h"

namespace vigilante {

class GameMapManager final {
 public:
  explicit GameMapManager(const b2Vec2& gravity);

  void update(const float delta);

  // @param tmxMapFilePath: the target .tmx file to load
  // @param afterLoadingGameMap: guaranteed to be called after the GameMap
  //                             has been loaded (optional).
  void loadGameMap(const std::string& tmxMapFilePath,
                   const std::function<void ()>& afterLoadingGameMap=[]() {});
  void destroyGameMap();
  bool rayCast(const b2Vec2& src, const b2Vec2& dst, const short categoryBitsToStop,
               const bool shouldDrawLine = false) const;

  bool isNpcAllowedToSpawn(const std::string& jsonFilePath) const;
  void setNpcAllowedToSpawn(const std::string& jsonFilePath, bool canSpawn);

  inline bool areNpcsAllowedToAct() const { return _areNpcsAllowedToAct; }
  inline void setNpcsAllowedToAct(bool npcsAllowedToAct) {
    _areNpcsAllowedToAct = npcsAllowedToAct;
  }

  bool hasSavedOpenedClosedState(const std::string& tmxMapFilePath,
                                 const GameMap::OpenableObjectType type,
                                 const int targetObjectId) const;
  bool isOpened(const std::string& tmxMapFilePath,
                const GameMap::OpenableObjectType type,
                const int targetPortalId) const;
  void setOpened(const std::string& tmxMapFilePath,
                 const GameMap::OpenableObjectType type,
                 const int targetPortalId,
                 const bool locked);

  inline ax::Layer* getParallaxLayer() const { return _parallaxLayer; }
  inline ax::Layer* getLayer() const { return _layer; }
  inline ax::Layer* getLightingLayer() const { return _lightingLayer; }
  inline ax::RenderTexture* getRenderTexture() const { return _renderTexture; }
  inline b2World* getWorld() const { return _world.get(); }
  inline GameMap* getGameMap() const { return _gameMap.get(); }
  inline Player* getPlayer() const { return _player.get(); }

 private:
  GameMap* doLoadGameMap(const std::string& tmxMapFilePath);
  std::string getOpenableObjectQueryKey(const std::string& tmxMapFilePath,
                                        const GameMap::OpenableObjectType type,
                                        const int targetObjectId) const;

  ax::Layer* _parallaxLayer{};
  ax::Layer* _layer{};
  ax::Layer* _lightingLayer{};
  ax::RenderTexture* _renderTexture{};
  std::unique_ptr<WorldContactListener> _worldContactListener;
  std::unique_ptr<b2World> _world;
  std::unique_ptr<GameMap> _gameMap;
  std::unique_ptr<Player> _player;

  std::unordered_set<std::string> _npcSpawningBlacklist;
  std::atomic<bool> _areNpcsAllowedToAct{true};

  // This includes:
  // 1. The unlock/lock state of all portals in all maps.
  // 2. The open/close state of all chests in all maps.
  std::unordered_map<std::string, bool> _allOpenableObjectStates;

  friend class GameState;
};

}  // namespace vigilante

#endif  // VIGILANTE_MAP_GAME_MAP_MANAGER_H_
