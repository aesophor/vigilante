// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Player.h"

#include <string>
#include <vector>

#include "AssetManager.h"
#include "Constants.h"
#include "input/InputManager.h"
#include "input/HotkeyManager.h"
#include "input/Keybindable.h"
#include "map/GameMapManager.h"
#include "skill/Skill.h"
#include "skill/BackDash.h"
#include "skill/ForwardSlash.h"
#include "skill/MagicalMissile.h"
#include "quest/KillTargetObjective.h"
#include "ui/Shade.h"
#include "ui/hud/Hud.h"
#include "ui/notifications/Notifications.h"
#include "util/CallbackUtil.h"
#include "util/CameraUtil.h"

using std::string;
using std::vector;
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
using vigilante::category_bits::kFeet;
using vigilante::category_bits::kMeleeWeapon;
using vigilante::category_bits::kItem;
using vigilante::category_bits::kGround;
using vigilante::category_bits::kPlatform;
using vigilante::category_bits::kWall;
using vigilante::category_bits::kPortal;
using vigilante::category_bits::kInteractableObject;
using vigilante::category_bits::kProjectile;

namespace vigilante {

Player::Player(const std::string& jsonFileName)
    : Character(jsonFileName), _questBook(asset_manager::kQuestsList) {}

void Player::showOnMap(float x, float y) {
  if (_isShownOnMap) {
    return;
  }
  _isShownOnMap = true;

  // Construct b2Body and b2Fixtures
  short bodyCategoryBits = kPlayer;
  short bodyMaskBits = kFeet | kEnemy | kMeleeWeapon | kProjectile;
  short feetMaskBits = kGround | kPlatform | kWall | kItem | kNpc | kPortal | kInteractableObject;
  short weaponMaskBits = kEnemy;
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
}

void Player::removeFromMap() {
  if (!_isShownOnMap) {
    return;
  }

  _isShownOnMap = false;

  if (!_isKilled) {
    _body->GetWorld()->DestroyBody(_body);
  }

  GameMapManager* gmMgr = GameMapManager::getInstance();
  gmMgr->getLayer()->removeChild(_bodySpritesheet);
  for (auto equipment : _equipmentSlots) {
    if (equipment) {
      Equipment::Type type = equipment->getEquipmentProfile().equipmentType;
      gmMgr->getLayer()->removeChild(_equipmentSpritesheets[type]);
    }
  }
}


void Player::handleInput() {
  if (_isSetToKill || _isAttacking || _isUsingSkill || _isSheathingWeapon || _isUnsheathingWeapon) {
    return;
  }

  auto inputMgr = InputManager::getInstance();

  if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_E)) {
    if (_interactableObject) {
      interact(_interactableObject);
    }
    return;
  } else if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_UP_ARROW)) {
    if (_portal) {
      interact(_portal);
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

  // Hotkeys
  for (auto keyCode : HotkeyManager::_kBindableKeys) {
    Keybindable* action = HotkeyManager::getInstance()->getHotkeyAction(keyCode);
    if (inputMgr->isKeyJustPressed(keyCode) && action) {
      if (dynamic_cast<Skill*>(action)) {
        activateSkill(dynamic_cast<Skill*>(action));
      } else if (dynamic_cast<Consumable*>(action)) {
        useItem(dynamic_cast<Consumable*>(action));
      }
    }
  }

  if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_Z)) {
    if (!_inRangeItems.empty()) {
      Item* item = *_inRangeItems.begin();
      string itemName = item->getItemProfile().name;
      int amount = item->getAmount();
      pickupItem(item);
      Notifications::getInstance()->show("Acquired item: " + itemName + ((amount > 1) ? (" (" + std::to_string(amount) + ")" + ".") : ""));
    }
  }

  if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_ALT)) {
    jump();
  }

  if (_isCrouching && !inputMgr->isKeyPressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
    getUp();
  }
}


void Player::inflictDamage(Character* target, int damage) {
  Character::inflictDamage(target, damage);
  camera_util::shake(8, .1f);

  if (target->isSetToKill()) {
    const string& targetName = target->getCharacterProfile().name;
    for (const auto objective : KillTargetObjective::getRelatedObjectives(targetName)) {
      dynamic_cast<KillTargetObjective*>(objective)->incrementCurrentAmount();
    }
    _questBook.update(Quest::Objective::Type::KILL);
  }
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

void Player::pickupItem(Item* item) {
  Character::pickupItem(item);
  _questBook.update(Quest::Objective::Type::COLLECT);
}


QuestBook& Player::getQuestBook() {
  return _questBook;
}

} // namespace vigilante
