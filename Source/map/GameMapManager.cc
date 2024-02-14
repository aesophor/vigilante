// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "GameMapManager.h"

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

void GameMapManager::loadGameMap(const string& tmxMapFilePath,
                                 const function<void ()>& afterLoadingGameMap) {
  auto shade = SceneManager::the().getCurrentScene<GameScene>()->getShade();
  shade->getImageView()->runAction(Sequence::create(
      FadeIn::create(Shade::kFadeInTime),
      CallFunc::create([this, shade, tmxMapFilePath, afterLoadingGameMap]() {
        doLoadGameMap(tmxMapFilePath);
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

GameMap* GameMapManager::doLoadGameMap(const string& tmxMapFilePath) {
  const string oldBgmFilePath = (_gameMap) ? _gameMap->getBgmFilePath() : "";

  destroyGameMap();
  _gameMap = std::make_unique<GameMap>(_world.get(), tmxMapFilePath);
  _gameMap->createObjects();
  _layer->addChild(_gameMap->getTmxTiledMap(), graphical_layers::kTmxTiledMap);

  if (!_player) {
    _player = _gameMap->createPlayer();
  }

  if (oldBgmFilePath != _gameMap->getBgmFilePath()) {
    Audio::the().playBgm(_gameMap->getBgmFilePath());
  }

  auto locationInfo = SceneManager::the().getCurrentScene<GameScene>()->getLocationInfo();
  locationInfo->updateLocationName();

  return _gameMap.get();
}

bool GameMapManager::rayCast(const b2Vec2 &src, const b2Vec2 &dst, const short categoryBitsToStop,
                             const bool shouldDrawLine) const {
  if (shouldDrawLine) {
    auto draw = ax::DrawNode::create();
    draw->drawLine(Point{src.x * kPpm, src.y * kPpm}, Point{dst.x * kPpm, dst.y * kPpm}, ax::Color4F::WHITE);
    _layer->addChild(draw, graphical_layers::kHud);
  }

  B2RayCastCallback cb{[categoryBitsToStop](b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) -> float {
    if (fixture->GetFilterData().categoryBits & categoryBitsToStop) {
      return 0;
    }
    return -1;
  }};
  _world->RayCast(&cb, src, dst);
  return cb.hasHit();
}

bool GameMapManager::isNpcAllowedToSpawn(const string& jsonFilePath) const {
  return _npcSpawningBlacklist.find(jsonFilePath) == _npcSpawningBlacklist.end();
}

void GameMapManager::setNpcAllowedToSpawn(const string& jsonFilePath, bool canSpawn) {
  if (!canSpawn && isNpcAllowedToSpawn(jsonFilePath)) {
    _npcSpawningBlacklist.insert(jsonFilePath);
  } else if (canSpawn && !isNpcAllowedToSpawn(jsonFilePath)) {
    _npcSpawningBlacklist.erase(jsonFilePath);
  }
}

bool GameMapManager::hasSavedOpenedClosedState(const string& tmxMapFilePath,
                                               const GameMap::OpenableObjectType type,
                                               const int targetPortalId) const {
  const string key = getOpenableObjectQueryKey(tmxMapFilePath, type, targetPortalId);
  auto it = _allOpenableObjectStates.find(key);
  return it != _allOpenableObjectStates.end();
}

bool GameMapManager::isOpened(const string& tmxMapFilePath,
                              const GameMap::OpenableObjectType type,
                              const int targetPortalId) const {
  const string key = getOpenableObjectQueryKey(tmxMapFilePath, type, targetPortalId);
  auto it = _allOpenableObjectStates.find(key);
  if (it == _allOpenableObjectStates.end()) {
    VGLOG(LOG_WARN, "Unable to find portal lock/unlock state, map [%s], portalId [%d].",
          tmxMapFilePath.c_str(), targetPortalId);
    return false;
  }

  return it->second;
}

void GameMapManager::setOpened(const string& tmxMapFilePath,
                               const GameMap::OpenableObjectType type,
                               const int targetPortalId,
                               const bool locked) {
  const string key = getOpenableObjectQueryKey(tmxMapFilePath, type, targetPortalId);
  _allOpenableObjectStates[key] = locked;
}

string GameMapManager::getOpenableObjectQueryKey(const string& tmxMapFilePath,
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

  return string_util::format("%s_%s_%d", tmxMapFilePath.c_str(), typeStr.c_str(), targetObjectId);
}

}  // namespace vigilante
