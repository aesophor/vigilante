#include "Player.h"

#include "GameAssetManager.h"
#include "ui/hud/Hud.h"
#include "util/CallbackUtil.h"
#include "util/CameraUtil.h"
#include "util/CategoryBits.h"
#include "util/Constants.h"

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
using vigilante::category_bits::kWall;
using vigilante::category_bits::kEnemy;
using vigilante::category_bits::kObject;
using vigilante::category_bits::kProjectile;

namespace vigilante {

Player::Player(const std::string& jsonFileName, float x, float y) : Character(x, y) {
  _profile.import("Resources/Database/player.json");

  short bodyCategoryBits = kPlayer;
  short bodyMaskBits = kEnemy | kMeleeWeapon | kProjectile;
  short feetMaskBits = kGround | kPlatform | kWall | kItem | kPortal;
  short weaponMaskBits = kEnemy | kObject;
  defineBody(b2BodyType::b2_dynamicBody, bodyCategoryBits, bodyMaskBits, feetMaskBits, weaponMaskBits, x, y);
  defineTexture(_profile.textureResPath, x, y);
}


void Player::inflictDamage(Character* target, int damage) {
  Character::inflictDamage(target, damage);
  camera_util::shake(8, .1f);
}

void Player::receiveDamage(Character* source, int damage) {
  Character::receiveDamage(source, damage);
  camera_util::shake(8, .1f);

  _bodyFixture->SetSensor(true);
  _isInvincible = true;

  callback_util::runAfter([&](){
    _bodyFixture->SetSensor(false);
    _isInvincible = false;
  }, 1.5f);

  Hud::getInstance()->updateStatusBars();
}

void Player::equip(Equipment* equipment) {
  Character::equip(equipment);
  Hud::getInstance()->updateEquippedWeapon();
}

void Player::unequip(Equipment::Type equipmentType) {
  Character::unequip(equipmentType);
  Hud::getInstance()->updateEquippedWeapon();
}

} // namespace vigilante
