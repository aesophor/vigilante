#include "Enemy.h"

#include "json/document.h"

#include "AssetManager.h"
#include "Constants.h"
#include "CategoryBits.h"
#include "GraphicalLayers.h"
#include "gameplay/ExpPointTable.h"
#include "item/Item.h"
#include "map/GameMapManager.h"
#include "ui/notification/NotificationManager.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/CallbackUtil.h"
#include "util/RandUtil.h"
#include "util/JsonUtil.h"

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
      Bot(this),
      _enemyProfile(jsonFileName) {}

void Enemy::update(float delta) {
  Character::update(delta);
  act(delta);
}

void Enemy::showOnMap(float x, float y) {
  if (_isShownOnMap) {
    return;
  }
  _isShownOnMap = true;
  GameMapManager::getInstance()->getGameMap()->getDynamicActors().insert(this);

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
    int& currentExp = source->getCharacterProfile().exp;
    currentExp += getCharacterProfile().exp;
    NotificationManager::getInstance()->show("Acquired " + std::to_string(getCharacterProfile().exp) + " exp.");

    int& currentLevel = source->getCharacterProfile().level;
    while (currentExp >= exp_point_table::getNextLevelExp(currentLevel)) {
      currentExp -= exp_point_table::getNextLevelExp(currentLevel);
      currentLevel++;
      NotificationManager::getInstance()->show("Congratulations! You are now level " + std::to_string(currentLevel) + ".");
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

} // namespace vigilante
