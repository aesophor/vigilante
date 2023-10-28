// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "GameMapManager.h"

#include <thread>

#include <box2d/box2d.h>

#include "Assets.h"
#include "Audio.h"
#include "CallbackManager.h"
#include "Constants.h"
#include "character/Npc.h"
#include "character/Player.h"
#include "item/Equipment.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "skill/MagicalMissile.h"
#include "util/B2BodyBuilder.h"
#include "util/B2RayCastUtil.h"
#include "util/StringUtil.h"

using namespace std;
USING_NS_AX;

namespace vigilante {

GameMapManager::GameMapManager(const b2Vec2& gravity)
    : _layer{Layer::create()},
      _parallaxLayer{Layer::create()},
      _worldContactListener{std::make_unique<WorldContactListener>()},
      _world{std::make_unique<b2World>(gravity)} {
  _world->SetAllowSleeping(true);
  _world->SetContinuousPhysics(true);
  _world->SetContactListener(_worldContactListener.get());
}

void GameMapManager::update(const float delta) {
  if (!_gameMap) {
    return;
  }
  _gameMap->update(delta);

  if (_player) {
    _player->update(delta);
    for (const auto& ally : _player->getAllies()) {
      ally->update(delta);
    }
  }
}

void GameMapManager::loadGameMap(const string& tmxMapFileName,
                                 const function<void ()>& afterLoadingGameMap) {
  auto shade = SceneManager::the().getCurrentScene<GameScene>()->getShade();
  shade->getImageView()->runAction(Sequence::create(
      FadeIn::create(Shade::kFadeInTime),
      CallFunc::create([this, shade, tmxMapFileName, afterLoadingGameMap]() {
        doLoadGameMap(tmxMapFileName);
        afterLoadingGameMap();
        setNpcsAllowedToAct(true);
      }),
      FadeOut::create(Shade::kFadeOutTime),
      nullptr
  ));
}

void GameMapManager::destroyGameMap() {
  setNpcsAllowedToAct(false);

  if (_player) {
    for (auto ally : _player->getAllies()) {
      ally->onMapChanged();
    }
  }

  if (_gameMap) {
    _parallaxLayer->removeAllChildren();
    _layer->removeChild(_gameMap->getTmxTiledMap());
    _gameMap.reset();
  }
}

GameMap* GameMapManager::doLoadGameMap(const string& tmxMapFileName) {
  const string oldBgmFileName = (_gameMap) ? _gameMap->getBgmFileName() : "";

  destroyGameMap();
  _gameMap = std::make_unique<GameMap>(_world.get(), tmxMapFileName);
  _gameMap->createObjects();
  _layer->addChild(_gameMap->getTmxTiledMap(), graphical_layers::kTmxTiledMap);

  if (!_player) {
    _player = _gameMap->createPlayer();
  }

  if (oldBgmFileName != _gameMap->getBgmFileName()) {
    Audio::the().stopBgm();
    Audio::the().playBgm(_gameMap->getBgmFileName());
  }

  return _gameMap.get();
}

bool GameMapManager::rayCast(const b2Vec2 &src, const b2Vec2 &dst, const bool shouldDrawLine) const {
  if (shouldDrawLine) {
    auto draw = ax::DrawNode::create();
    draw->drawLine(Point{src.x * kPpm, src.y * kPpm}, Point{dst.x * kPpm, dst.y * kPpm}, ax::Color4F::WHITE);
    _layer->addChild(draw, graphical_layers::kHud);
  }

  RayCastCallback cb{};
  _world->RayCast(&cb, src, dst);
  return cb.hasHit();
}

bool GameMapManager::isNpcAllowedToSpawn(const string& jsonFileName) const {
  return _npcSpawningBlacklist.find(jsonFileName) == _npcSpawningBlacklist.end();
}

void GameMapManager::setNpcAllowedToSpawn(const string& jsonFileName, bool canSpawn) {
  if (!canSpawn && isNpcAllowedToSpawn(jsonFileName)) {
    _npcSpawningBlacklist.insert(jsonFileName);
  } else if (canSpawn && !isNpcAllowedToSpawn(jsonFileName)) {
    _npcSpawningBlacklist.erase(jsonFileName);
  }
}

bool GameMapManager::hasSavedOpenedClosedState(const string& tmxMapFileName,
                                               const GameMap::OpenableObjectType type,
                                               const int targetPortalId) const {
  const string key = getOpenableObjectQueryKey(tmxMapFileName, type, targetPortalId);
  auto it = _allOpenableObjectStates.find(key);
  return it != _allOpenableObjectStates.end();
}

bool GameMapManager::isOpened(const string& tmxMapFileName,
                              const GameMap::OpenableObjectType type,
                              const int targetPortalId) const {
  const string key = getOpenableObjectQueryKey(tmxMapFileName, type, targetPortalId);
  auto it = _allOpenableObjectStates.find(key);
  if (it == _allOpenableObjectStates.end()) {
    VGLOG(LOG_WARN, "Unable to find portal lock/unlock state, map [%s], portalId [%d].",
        tmxMapFileName.c_str(), targetPortalId);
    return false;
  }

  return it->second;
}

void GameMapManager::setOpened(const string& tmxMapFileName,
                               const GameMap::OpenableObjectType type,
                               const int targetPortalId,
                               const bool locked) {
  const string key = getOpenableObjectQueryKey(tmxMapFileName, type, targetPortalId);
  _allOpenableObjectStates[key] = locked;
}

string GameMapManager::getOpenableObjectQueryKey(const string& tmxMapFileName,
                                                 const GameMap::OpenableObjectType type,
                                                 const int targetObjectId) const {
  string typeStr;
  switch (type) {
    case GameMap::OpenableObjectType::PORTAL:
      typeStr = "p";
      break;
    case GameMap::OpenableObjectType::CHEST:
      typeStr = "c";
      break;
    default:
      break;
  }

  return string_util::format("%s_%s_%d", tmxMapFileName.c_str(), typeStr.c_str(), targetObjectId);
}

}  // namespace vigilante
