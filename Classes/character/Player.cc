#include "Player.h"

#include "AssetManager.h"
#include "Constants.h"
#include "CategoryBits.h"
#include "GraphicalLayers.h"
#include "input/GameInputManager.h"
#include "map/GameMapManager.h"
#include "skill/Skill.h"
#include "skill/BackDash.h"
#include "skill/ForwardSlash.h"
#include "skill/MagicalMissile.h"
#include "ui/Shade.h"
#include "ui/hud/Hud.h"
#include "ui/notification/NotificationManager.h"
#include "util/CallbackUtil.h"
#include "util/CameraUtil.h"

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
using cocos2d::FadeIn;
using cocos2d::FadeOut;
using cocos2d::CallFunc;
using cocos2d::Sequence;
using cocos2d::EventKeyboard;
using vigilante::category_bits::kPlayer;
using vigilante::category_bits::kEnemy;
using vigilante::category_bits::kNpc;
using vigilante::category_bits::kPortal;
using vigilante::category_bits::kMeleeWeapon;
using vigilante::category_bits::kItem;
using vigilante::category_bits::kGround;
using vigilante::category_bits::kPlatform;
using vigilante::category_bits::kWall;
using vigilante::category_bits::kEnemy;
using vigilante::category_bits::kObject;
using vigilante::category_bits::kProjectile;

namespace vigilante {

Player::Player(const std::string& jsonFileName) : Character(jsonFileName) {}

void Player::showOnMap(float x, float y) {
  if (!_isShownOnMap) {
    // Construct b2Body and b2Fixtures
    short bodyCategoryBits = kPlayer;
    short bodyMaskBits = kEnemy | kMeleeWeapon | kProjectile;
    short feetMaskBits = kGround | kPlatform | kWall | kItem | kPortal | kNpc;
    short weaponMaskBits = kEnemy | kObject;
    defineBody(b2BodyType::b2_dynamicBody, bodyCategoryBits, bodyMaskBits, feetMaskBits, weaponMaskBits, x, y);

    // Load sprites, spritesheets, and animations, and then add them to GameMapManager layer.
    defineTexture(_characterProfile.textureResDir, x, y);
    GameMapManager* gmMgr = GameMapManager::getInstance();
    gmMgr->getLayer()->addChild(_bodySpritesheet, graphical_layers::kPlayerBody);
    for (auto equipment : _equipmentSlots) {
      if (equipment) {
        Equipment::Type type = equipment->getEquipmentProfile().equipmentType;
        gmMgr->getLayer()->addChild(_equipmentSpritesheets[type], graphical_layers::kEquipment - type);
      }
    }

    _isShownOnMap = true;
  }
}

void Player::handleInput() {
  if (_isSetToKill || _isAttacking || _isUsingSkill || _isSheathingWeapon || _isUnsheathingWeapon) {
    return;
  }

  auto inputMgr = GameInputManager::getInstance();
  if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_UP_ARROW)) {
    if (_portal) {
      Shade::getInstance()->getImageView()->runAction(Sequence::create(
        FadeIn::create(Shade::_kFadeInTime),
        CallFunc::create([=]() {
          GameMap::Portal* portal = _portal;
          portal->interact();
        }),
        FadeOut::create(Shade::_kFadeOutTime),
        nullptr
      ));
    }
    return;
  }

  if (inputMgr->isKeyPressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
    crouch();
    if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_LEFT_ALT)) {
      jumpDown();
    }
  }

  if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_LEFT_CTRL)) {
    if (!_isWeaponSheathed) {
      attack();
    } else {
      NotificationManager::getInstance()->show("You haven't equipped a weapon yet.");
    }
  }

  if (inputMgr->isKeyPressed(EventKeyboard::KeyCode::KEY_LEFT_ARROW)) {
    moveLeft();
  } else if (inputMgr->isKeyPressed(EventKeyboard::KeyCode::KEY_RIGHT_ARROW)) {
    moveRight();
  }

  if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_R)) {
    if (_equipmentSlots[Equipment::Type::WEAPON]
        && _isWeaponSheathed && !_isUnsheathingWeapon) {
      unsheathWeapon();
    } else if (!_isWeaponSheathed && !_isSheathingWeapon) {
      sheathWeapon();
    }
  }

  if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_X)) {
    Skill* skill = new MagicalMissile("Resources/Database/skill/ice_spike.json", this);
    useSkill(skill);
  } else if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_C)) {
    useSkill(new ForwardSlash("Resources/Database/skill/forward_slash.json", this));
  } else if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_S)) {
    useSkill(new BackDash("Resources/Database/skill/back_dash.json", this));
  }

  if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_Z)) {
    if (!_inRangeItems.empty()) {
      Item* item = *_inRangeItems.begin();
      pickupItem(item);
      NotificationManager::getInstance()->show("Acquired item: " + item->getItemProfile().name + ".");
    }
  }

  if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_LEFT_ALT)) {
    jump();
  }

  if (_isCrouching && !inputMgr->isKeyPressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
    getUp();
  }
}


void Player::inflictDamage(Character* target, int damage) {
  Character::inflictDamage(target, damage);
  camera_util::shake(8, .1f);
}

void Player::receiveDamage(Character* source, int damage) {
  Character::receiveDamage(source, damage);
  camera_util::shake(8, .1f);

  _fixtures[FixtureType::BODY]->SetSensor(true);
  _isInvincible = true;

  callback_util::runAfter([&](){
    _fixtures[FixtureType::BODY]->SetSensor(false);
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
