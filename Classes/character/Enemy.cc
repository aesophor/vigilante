#include "Enemy.h"

#include "json/document.h"

#include "GameAssetManager.h"
#include "map/GameMapManager.h"
#include "ui/notification/NotificationManager.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/CallbackUtil.h"
#include "util/CategoryBits.h"
#include "util/Constants.h"
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
using vigilante::category_bits::kPortal;
using vigilante::category_bits::kEnemy;
using vigilante::category_bits::kMeleeWeapon;
using vigilante::category_bits::kItem;
using vigilante::category_bits::kGround;
using vigilante::category_bits::kPlatform;
using vigilante::category_bits::kCliffMarker;
using vigilante::category_bits::kWall;
using vigilante::category_bits::kEnemy;
using vigilante::category_bits::kObject;
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
  if (!_isShownOnMap) {
    // Construct b2Body and b2Fixtures.
    short bodyCategoryBits = kEnemy;
    short bodyMaskBits = kPlayer | kMeleeWeapon | kCliffMarker | kProjectile;
    short feetMaskBits = kGround | kPlatform | kWall | kItem | kPortal;
    short weaponMaskBits = kPlayer | kObject;
    defineBody(b2BodyType::b2_dynamicBody, bodyCategoryBits, bodyMaskBits, feetMaskBits, weaponMaskBits, x, y);

    // Load sprites, spritesheets, and animations, and then add them to GameMapManager layer.
    defineTexture(_characterProfile.textureResDir, x, y);
    GameMapManager* gmMgr = GameMapManager::getInstance();
    gmMgr->getLayer()->addChild(_bodySpritesheet, 32);
    for (auto equipment : _equipmentSlots) {
      if (equipment) {
        Equipment::Type type = equipment->getEquipmentProfile().equipmentType;
        gmMgr->getLayer()->addChild(_equipmentSpritesheets[type], 33);
      }
    }

    _isShownOnMap = true;
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
    NotificationManager::getInstance()->show("Gained 25 exp.");
  }
}

Enemy::Profile& Enemy::getEnemyProfile() {
  return _enemyProfile;
}


Enemy::Profile::Profile(const string& jsonFileName) {
  Document json = json_util::parseJson(jsonFileName);
  
  const auto& dropItemsMap = json["dropItems"].GetObject();

  if (!dropItemsMap.ObjectEmpty()) {
    for (const auto& keyValue : dropItemsMap) {
      dropItems.insert({keyValue.name.GetString(), keyValue.value.GetFloat()});
    }
  }
}

} // namespace vigilante
