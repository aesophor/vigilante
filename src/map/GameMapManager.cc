// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "GameMapManager.h"

#include <thread>

#include <Box2D/Box2D.h>
#include "std/make_unique.h"
#include "AssetManager.h"
#include "CallbackManager.h"
#include "Constants.h"
#include "character/Npc.h"
#include "character/Player.h"
#include "item/Equipment.h"
#include "skill/MagicalMissile.h"
#include "ui/Shade.h"
#include "ui/pause_menu/PauseMenu.h"
#include "util/box2d/b2BodyBuilder.h"

using std::string;
using std::thread;
using std::function;
using cocos2d::Director;
using cocos2d::Layer;
using cocos2d::TMXTiledMap;
using cocos2d::TMXObjectGroup;
using cocos2d::Sequence;
using cocos2d::FadeIn;
using cocos2d::FadeOut;
using cocos2d::CallFunc;

namespace vigilante {

GameMapManager* GameMapManager::getInstance() {
  static GameMapManager instance({0, kGravity});
  return &instance;
}

GameMapManager::GameMapManager(const b2Vec2& gravity)
    : _layer(Layer::create()),
      _worldContactListener(std::make_unique<WorldContactListener>()),
      _world(std::make_unique<b2World>(gravity)),
      _fxMgr(std::make_unique<FxManager>(_layer)),
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
        FadeOut::create(Shade::_kFadeOutTime)
    ));

    // Resume NPCs to act.
    Npc::setNpcsAllowedToAct(true);
  };

  // 1. Fade in the shade
  // 2. Create another thread which executes the above lambda independently.
  Shade::getInstance()->getImageView()->runAction(Sequence::createWithTwoActions(
      FadeIn::create(Shade::_kFadeInTime),
      CallFunc::create([workerThreadLambda]() {
        thread(workerThreadLambda).detach();
      })
  ));
}

GameMap* GameMapManager::doLoadGameMap(const string& tmxMapFileName) {
  // Remove deceased party member from player's party, remove their
  // b2body and texture, and add them to the party's deceasedMember unordered_set.
  if (_player) {
    for (auto ally : _player->getAllies()) {
      if (ally->isKilled()) {
        _player->getParty()->dismiss(ally, /*addToMap=*/false);
      }
    }
  }

  // Clean up previous GameMap.
  if (_gameMap) {
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
    PauseMenu::getInstance()->setPlayer(_player.get());
  }

  return _gameMap.get();
}


void GameMapManager::createDustFx(Character* character) {
  const b2Vec2& feetPos = character->getBody()->GetPosition();
  float x = feetPos.x * kPpm;// - 32.f / kPpm / 2;
  float y = (feetPos.y - .1f) * kPpm;// - 32.f / kPpm / .065f;
  _fxMgr->createFx("Texture/fx/dust", "white", x, y, 1, 10.0f);
}


Layer* GameMapManager::getLayer() const {
  return _layer;
}

b2World* GameMapManager::getWorld() const {
  return _world.get();
}

FxManager* GameMapManager::getFxManager() const {
  return _fxMgr.get();
}

GameMap* GameMapManager::getGameMap() const {
  return _gameMap.get();
}

Player* GameMapManager::getPlayer() const {
  return _player.get();
}

}  // namespace vigilante
