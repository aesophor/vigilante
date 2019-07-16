// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_GAMEMAP_MANAGER_H_
#define VIGILANTE_GAMEMAP_MANAGER_H_

#include <set>
#include <string>
#include <memory>

#include "cocos2d.h"
#include "Box2D/Box2D.h"

#include "GameMap.h"
#include "WorldContactListener.h"
#include "FxManager.h"
#include "Controllable.h"
#include "character/Character.h"
#include "item/Item.h"

namespace vigilante {

class Player;

class GameMapManager {
 public:
  static GameMapManager* getInstance();
  virtual ~GameMapManager() = default;

  void update(float delta);

  GameMap* getGameMap() const;
  void loadGameMap(const std::string& tmxMapFileName);

  Player* getPlayer() const;
  b2World* getWorld() const;

  cocos2d::Layer* getLayer() const;

  void createDustFx(Character* character);

 private:
  static GameMapManager* _instance;
  GameMapManager(const b2Vec2& gravity);

  cocos2d::Layer* _layer;
  std::unique_ptr<WorldContactListener> _worldContactListener;
  std::unique_ptr<b2World> _world;
  std::unique_ptr<FxManager> _fxMgr;
  std::unique_ptr<GameMap> _gameMap;
  std::unique_ptr<Player> _player;
};

} // namespace vigilante

#endif // VIGILANTE_GAMEMAP_MANAGER_H_
