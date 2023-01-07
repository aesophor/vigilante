// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "GameMapManager.h"

#include <thread>

#include <Box2D/Box2D.h>

#include "AudioManager.h"
#include "Assets.h"
#include "CallbackManager.h"
#include "Constants.h"
#include "character/Npc.h"
#include "character/Player.h"
#include "item/Equipment.h"
#include "skill/MagicalMissile.h"
#include "ui/Shade.h"
#include "ui/pause_menu/PauseMenu.h"
#include "util/box2d/b2BodyBuilder.h"

using namespace std;
USING_NS_CC;

namespace vigilante {

GameMapManager* GameMapManager::getInstance() {
  static GameMapManager instance({0, kGravity});
  return &instance;
}

GameMapManager::GameMapManager(const b2Vec2& gravity)
    : _layer(Layer::create()),
      _worldContactListener(std::make_unique<WorldContactListener>()),
      _world(std::make_unique<b2World>(gravity)),
      _gameMap(),
      _player() {
  _world->SetAllowSleeping(true);
  _world->SetContinuousPhysics(true);
  _world->SetContactListener(_worldContactListener.get());
}

void GameMapManager::update(float delta) {
  for (auto& actor : _gameMap->_dynamicActors) {
    actor->update(delta);
  }

  if (_player) {
    _player->update(delta);

    for (const auto& ally : _player->getAllies()) {
      ally->update(delta);
    }
  }
}

void GameMapManager::loadGameMap(const string& tmxMapFileName,
                                 const function<void ()>& afterLoadingGameMap) {
  auto workerThreadLambda = [this, tmxMapFileName, afterLoadingGameMap]() {
    // Pauses all NPCs from acting, preventing new callbacks
    // from being generated.
    Npc::setNpcsAllowedToAct(false);

    // Block this thread with a spinlock until all callbacks have finished.
    while (CallbackManager::getInstance()->getPendingCount() > 0);

    // No pending callbacks. Now it's safe to load the new GameMap.
    Shade::getInstance()->getImageView()->runAction(Sequence::createWithTwoActions(
        CallFunc::create([this, tmxMapFileName, afterLoadingGameMap]() {
          doLoadGameMap(tmxMapFileName);
          afterLoadingGameMap();
        }),
        FadeOut::create(Shade::kFadeOutTime)
    ));

    // Resume NPCs to act.
    Npc::setNpcsAllowedToAct(true);
  };

  // 1. Fade in the shade
  // 2. Create another thread which executes the above lambda independently.
  Shade::getInstance()->getImageView()->runAction(Sequence::createWithTwoActions(
      FadeIn::create(Shade::kFadeInTime),
      CallFunc::create([workerThreadLambda]() {
        thread(workerThreadLambda).detach();
      })
  ));
}

GameMap* GameMapManager::doLoadGameMap(const string& tmxMapFileName) {
  string oldBgmFileName;

  // Remove deceased party member from player's party,
  // and remove their b2body and texture.
  if (_player) {
    for (auto ally : _player->getAllies()) {
      ally->onMapChanged();
    }
  }

  // Clean up previous GameMap.
  if (_gameMap) {
    oldBgmFileName = _gameMap->getBgmFileName();
    _layer->removeChild(_gameMap->getTmxTiledMap());
    _gameMap->deleteObjects();
    _gameMap.reset();  // deletes the underlying GameMap object and _gameMap = nullptr.
  }

  // Load the new GameMap.
  _gameMap = std::make_unique<GameMap>(_world.get(), tmxMapFileName);
  _gameMap->createObjects();
  _layer->addChild(_gameMap->getTmxTiledMap(), graphical_layers::kTmxTiledMap);

  // If the player object hasn't been created yet, then spawn it.
  if (!_player) {
    _player = _gameMap->createPlayer();
  }

  if (oldBgmFileName != _gameMap->getBgmFileName()) {
    AudioManager::getInstance()->stopBgm();
    AudioManager::getInstance()->playBgm(_gameMap->getBgmFileName());
  }

  return _gameMap.get();
}

}  // namespace vigilante
