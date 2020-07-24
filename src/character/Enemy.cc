// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Enemy.h"

#include <set>

#include <json/document.h>
#include "AssetManager.h"
#include "Constants.h"
#include "character/Player.h"
#include "gameplay/ExpPointTable.h"
#include "item/Item.h"
#include "map/GameMapManager.h"
#include "ui/notifications/Notifications.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/CallbackUtil.h"
#include "util/RandUtil.h"
#include "util/JsonUtil.h"

using std::set;
using std::string;
using cocos2d::Vector;
using cocos2d::Director;
using cocos2d::Repeat;
using cocos2d::RepeatForever;
using cocos2d::Animation;
using cocos2d::Animate;
using cocos2d::Action;
using cocos2d::Sprite;
using cocos2d::SpriteFrame;
using cocos2d::SpriteFrameCache;
using cocos2d::SpriteBatchNode;
using vigilante::category_bits::kPlayer;
using vigilante::category_bits::kEnemy;
using vigilante::category_bits::kFeet;
using vigilante::category_bits::kMeleeWeapon;
using vigilante::category_bits::kItem;
using vigilante::category_bits::kGround;
using vigilante::category_bits::kPlatform;
using vigilante::category_bits::kCliffMarker;
using vigilante::category_bits::kWall;
using vigilante::category_bits::kInteractableObject;
using vigilante::category_bits::kProjectile;
using rapidjson::Document;

namespace vigilante {

Enemy::Enemy(const string& jsonFileName)
    : Character(jsonFileName),
      _enemyProfile(jsonFileName),
      _isMovingRight(),
      _moveDuration(),
      _moveTimer(),
      _waitDuration(),
      _waitTimer(),
      _lastTraveledDistance(),
      _calculateDistanceTimer() {}

void Enemy::update(float delta) {
  Character::update(delta);
  act(delta);
}

void Enemy::showOnMap(float x, float y) {
  if (_isShownOnMap) {
    return;
  }
  _isShownOnMap = true;

  // Construct b2Body and b2Fixtures.
  short bodyCategoryBits = kEnemy;
  short bodyMaskBits = kFeet | kPlayer | kMeleeWeapon | kCliffMarker | kProjectile;
  short feetMaskBits = kGround | kPlatform | kWall | kItem | kInteractableObject;
  short weaponMaskBits = kPlayer;
  defineBody(b2BodyType::b2_dynamicBody, bodyCategoryBits, bodyMaskBits, feetMaskBits, weaponMaskBits, x, y);

  // Load sprites, spritesheets, and animations, and then add them to GameMapManager layer.
  defineTexture(_characterProfile.textureResDir, x, y);
  GameMapManager* gmMgr = GameMapManager::getInstance();
  gmMgr->getLayer()->addChild(_bodySpritesheet, graphical_layers::kEnemyBody);
  for (auto equipment : _equipmentSlots) {
    if (equipment) {
      Equipment::Type type = equipment->getEquipmentProfile().equipmentType;
      gmMgr->getLayer()->addChild(_equipmentSpritesheets[type], graphical_layers::kEquipment - type);
    }
  }
}

void Enemy::import(const string& jsonFileName) {
  Character::import(jsonFileName);
  _enemyProfile = Enemy::Profile(jsonFileName);
}

void Enemy::receiveDamage(Character* source, int damage) {
  Character::receiveDamage(source, damage);
  _isAlerted = true;

  if (_isSetToKill) {
    // Give source character exp point.
    int& sourceCharacterExp = source->getCharacterProfile().exp;
    int& sourceCharacterLevel = source->getCharacterProfile().level;

    sourceCharacterExp += getCharacterProfile().exp;
    Notifications::getInstance()->show("Acquired " + std::to_string(getCharacterProfile().exp) + " exp.");

    while (sourceCharacterExp >= exp_point_table::getNextLevelExp(sourceCharacterLevel)) {
      sourceCharacterExp -= exp_point_table::getNextLevelExp(sourceCharacterLevel);
      sourceCharacterLevel++;
      Notifications::getInstance()->show("Congratulations! You are now level " + std::to_string(sourceCharacterLevel) + ".");
    }

    // Drop items. (Here we'll use a callback to drop items
    // since creating fixtures during collision callback will crash)
    // See: https://github.com/libgdx/libgdx/issues/2730
    callback_util::runAfter([=]() {
      for (const auto& i : _enemyProfile.droppedItems) {
        const string& itemJson = i.first;
        float dropChance = i.second.chance;

        float randChance = rand_util::randInt(0, 100);
        if (randChance <= dropChance) {
          float x = _body->GetPosition().x;
          float y = _body->GetPosition().y;
          int amount = rand_util::randInt(i.second.minAmount, i.second.maxAmount);
          GameMapManager::getInstance()->getGameMap()->spawnItem(itemJson, x * kPpm, y * kPpm, amount);
        }
      }
    }, .2f);
  }
}


void Enemy::act(float delta) {
  if (isKilled() || isSetToKill() || isAttacking()) {
    return;
  }

  set<Character*>& inRangeTargets = getInRangeTargets();
  Character* lockedOnTarget = getLockedOnTarget();

  if (isAlerted() && lockedOnTarget && !lockedOnTarget->isSetToKill()) {
    if (!inRangeTargets.empty()) { // is target within the attack range?
      attack();
      if (inRangeTargets.empty()) {
        setLockedOnTarget(nullptr);
      }
    } else if (std::abs(getBody()->GetPosition().x - lockedOnTarget->getBody()->GetPosition().x) > .25f) {
      // If the target isn't within attack range, move toward it until attackable
      moveToTarget(lockedOnTarget);
      jumpIfStucked(delta, .1f);
    }
  } else {
    moveRandomly(delta, 0, 5, 0, 5);
  }
}

void Enemy::moveToTarget(Character* target) {
  b2Body* thisBody = getBody();
  b2Body* targetBody = target->getBody();

  if (thisBody->GetPosition().x > targetBody->GetPosition().x) {
    moveLeft();
  } else {
    moveRight();
  }
}

void Enemy::moveRandomly(float delta, int minMoveDuration, int maxMoveDuration, int minWaitDuration, int maxWaitDuration) {
  // If the character has finished moving and waiting, regenerate random values for
  // _moveDuration and _waitDuration within the specified range.
  if (_moveTimer >= _moveDuration && _waitTimer >= _waitDuration) {
    _isMovingRight = static_cast<bool>(rand_util::randInt(0, 1));
    _moveDuration = rand_util::randInt(minMoveDuration, maxMoveDuration);
    _waitDuration = rand_util::randInt(minWaitDuration, maxWaitDuration);
    _moveTimer = 0;
    _waitTimer = 0;
  }

  if (_moveTimer < _moveDuration) {
    if (_isMovingRight) {
      moveRight();
    } else {
      moveLeft();
    }
    // Make sure the character doesn't get stucked somewhere along the way.
    jumpIfStucked(delta, .5f);
    _moveTimer += delta;
  } else {
    _waitTimer += delta;
  }
}

void Enemy::jumpIfStucked(float delta, float checkInterval) {
  b2Body* thisBody = getBody();

  if (_calculateDistanceTimer > checkInterval) {
    _lastTraveledDistance = std::abs(thisBody->GetPosition().x - _lastStoppedPosition.x);
    _lastStoppedPosition.Set(thisBody->GetPosition().x, thisBody->GetPosition().y);
    if (_lastTraveledDistance == 0) {
      jump();
    }
    _calculateDistanceTimer = 0;
  } else {
    _calculateDistanceTimer += delta;
  }
}

void Enemy::reverseDirection() {
  _isMovingRight = !_isMovingRight;
}


Enemy::Profile& Enemy::getEnemyProfile() {
  return _enemyProfile;
}


Enemy::Profile::Profile(const string& jsonFileName) {
  Document json = json_util::parseJson(jsonFileName);
  const auto& droppedItemsMap = json["droppedItems"].GetObject();

  if (!droppedItemsMap.ObjectEmpty()) {
    for (const auto& keyValue : droppedItemsMap) {
      const auto& droppedItemDataJson = keyValue.value.GetObject();

      DroppedItemData droppedItemData;
      droppedItemData.chance = droppedItemDataJson["chance"].GetInt();
      droppedItemData.minAmount = droppedItemDataJson["minAmount"].GetInt();
      droppedItemData.maxAmount = droppedItemDataJson["maxAmount"].GetInt();

      droppedItems.insert({keyValue.name.GetString(), droppedItemData});
    }
  }
}

}  // namespace vigilante
