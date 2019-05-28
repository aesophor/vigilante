#include "Enemy.h"

#include "json/document.h"

#include "Constants.h"
#include "CategoryBits.h"
#include "GraphicalLayers.h"
#include "GameAssetManager.h"
#include "map/GameMapManager.h"
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
    gmMgr->getLayer()->addChild(_bodySpritesheet, graphical_layers::kEnemyBody);
    for (auto equipment : _equipmentSlots) {
      if (equipment) {
        Equipment::Type type = equipment->getEquipmentProfile().equipmentType;
        gmMgr->getLayer()->addChild(_equipmentSpritesheets[type], graphical_layers::kEquipment - type);
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
    // Drop items. (Here we'll use a callback to drop items
    // since creating fixtures during collision callback will crash)
    // See: https://github.com/libgdx/libgdx/issues/2730
    callback_util::runAfter([=]() {
      for (const auto& item : _enemyProfile.dropItems) {
        const string& itemResDir = item.first;
        float dropChance = item.second;

        float randChance = rand_util::randInt(0, 100);
        if (randChance <= dropChance) {
          Item* i = new Equipment(itemResDir);
          float x = _body->GetPosition().x;
          float y = _body->GetPosition().y;
          i->showOnMap(x * kPpm, y * kPpm);
          GameMapManager::getInstance()->getGameMap()->getDynamicActors().insert(i);

          float offsetX = rand_util::randFloat(-.3f, .3f);
          float offsetY = 3.0f;
          i->getBody()->ApplyLinearImpulse({offsetX, offsetY}, i->getBody()->GetWorldCenter(), true);
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
  
  const auto& dropItemsMap = json["dropItems"].GetObject();

  if (!dropItemsMap.ObjectEmpty()) {
    for (const auto& keyValue : dropItemsMap) {
      dropItems.insert({keyValue.name.GetString(), keyValue.value.GetFloat()});
    }
  }
}

} // namespace vigilante
