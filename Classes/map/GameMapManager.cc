#include "GameMapManager.h"

#include "Box2D/Box2D.h"

#include "GameAssetManager.h"
#include "character/Player.h"
#include "character/Enemy.h"
#include "item/Equipment.h"
#include "spell/IceSpike.h"
#include "map/fx/Dust.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/CategoryBits.h"
#include "util/Constants.h"

using std::set;
using std::string;
using std::unique_ptr;
using cocos2d::Director;
using cocos2d::Layer;
using cocos2d::TMXTiledMap;
using cocos2d::TMXObjectGroup;

namespace vigilante {

GameMapManager* GameMapManager::_instance = nullptr;

GameMapManager* GameMapManager::getInstance() {
  if (!_instance) {
    _instance = new GameMapManager({0, kGravity});
  }
  return _instance;
}

GameMapManager::GameMapManager(const b2Vec2& gravity)
    : _layer(Layer::create()),
      _worldContactListener(new WorldContactListener()),
      _world(new b2World(gravity)),
      _gameMap(),
      _player() {
  _world->SetAllowSleeping(true);
  _world->SetContinuousPhysics(true);
  _world->SetContactListener(_worldContactListener.get());
}

void GameMapManager::update(float delta) {
  if (_player) {
    _player->update(delta);
  }

  for (auto npc : _gameMap->getNpcs()) {
    npc->update(delta);
  }
  for (auto item : _gameMap->getDroppedItems()) {
    item->update(delta);
  }
  for (auto spell : _gameMap->getInUseSpells()) {
    spell->update(delta);
  }
}


GameMap* GameMapManager::getGameMap() const {
  return _gameMap.get();
}

void GameMapManager::loadGameMap(const string& tmxMapFileName) {
  // Clean up previous GameMap.
  if (_gameMap) {
    _layer->removeChild(_gameMap->getTmxTiledMap());
    _gameMap.reset(); // deletes the underlying GameMap object
  }

  _gameMap = unique_ptr<GameMap>(new GameMap(_world.get(), tmxMapFileName));
  _layer->addChild(_gameMap->getTmxTiledMap(), 0);

  // If the player object hasn't been created, spawn it.
  if (!_player) {
    _player = unique_ptr<Player>(_gameMap->createPlayer());
  }
}


Player* GameMapManager::getPlayer() const {
  return _player.get();
}

b2World* GameMapManager::getWorld() const {
  return _world.get();
}

Layer* GameMapManager::getLayer() const {
  return _layer;
}


void GameMapManager::createDustFx(Character* character) {
	auto feetPos = character->getBody()->GetPosition();
	float dustX = feetPos.x;// - 32.f / kPpm / 2;
	float dustY = feetPos.y - .1f;// - 32.f / kPpm / .065f;
  Dust::create(_layer, dustX, dustY);
}

} // namespace vigilante
