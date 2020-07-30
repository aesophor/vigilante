// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Player.h"

#include <string>
#include <vector>

#include "AssetManager.h"
#include "CallbackManager.h"
#include "Constants.h"
#include "character/Party.h"
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
#include "util/CameraUtil.h"
#include "util/StringUtil.h"

#define PLAYER_BODY_CATEGORY_BITS kPlayer
#define PLAYER_BODY_MASK_BITS kFeet | kEnemy | kMeleeWeapon | kProjectile
#define PLAYER_FEET_MASK_BITS kGround | kPlatform | kWall | kItem | kNpc | kPortal | kInteractable
#define PLAYER_WEAPON_MASK_BITS kEnemy

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
using vigilante::category_bits::kInteractable;
using vigilante::category_bits::kProjectile;

namespace vigilante {

Player::Player(const std::string& jsonFileName)
    : Character(jsonFileName),
      _questBook(asset_manager::kQuestsList) {
  // The player has a party (team) with no other members by default.
  _party = std::make_shared<Party>(this);
}


void Player::showOnMap(float x, float y) {
  if (_isShownOnMap) {
    return;
  }
  _isShownOnMap = true;

  // Construct b2Body and b2Fixtures
  defineBody(b2BodyType::b2_dynamicBody, x, y,
             PLAYER_BODY_CATEGORY_BITS,
             PLAYER_BODY_MASK_BITS,
             PLAYER_FEET_MASK_BITS,
             PLAYER_WEAPON_MASK_BITS);

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


void Player::inflictDamage(Character* target, int damage) {
  Character::inflictDamage(target, damage);
  camera_util::shake(8, .1f);
  
  if (target->isSetToKill()) {
    Notifications::getInstance()->show(
        string_util::format("Acquired %d exp", target->getCharacterProfile().exp)
    );

    updateKillTargetObjectives(target);
  }
}

void Player::receiveDamage(Character* source, int damage) {
  Character::receiveDamage(source, damage);
  camera_util::shake(8, .1f);

  _fixtures[FixtureType::BODY]->SetSensor(true);
  _isInvincible = true;

  CallbackManager::getInstance()->runAfter([&](){
    _fixtures[FixtureType::BODY]->SetSensor(false);
    _isInvincible = false;
  }, 1.0f);

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

  Notifications::getInstance()->show((item->getAmount() > 1) ?
      string_util::format("Acquired item: %s (%d).", item->getName().c_str(), item->getAmount()) :
      string_util::format("Acquired item: %s.", item->getName().c_str())
  );

  _questBook.update(Quest::Objective::Type::COLLECT);
}

void Player::addExp(const int exp) {
  int originalLevel = _characterProfile.level;
  Character::addExp(exp);

  if (_characterProfile.level > originalLevel) {
    Notifications::getInstance()->show(
        string_util::format("Congrats! You are now level %d.", _characterProfile.level));
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
      pickupItem(*_inRangeItems.begin());
    }
  }

  if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_ALT)) {
    jump();
  }

  if (_isCrouching && !inputMgr->isKeyPressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
    getUp();
  }
}


void Player::updateKillTargetObjectives(Character* killedCharacter) {
  if (!killedCharacter->isSetToKill()) {
    return;
  }

  for (auto quest : _questBook.getInProgressQuests()) {
    auto ko = dynamic_cast<KillTargetObjective*>(quest->getCurrentStage().objective.get());
    if (ko && ko->getCharacterName() == killedCharacter->getCharacterProfile().name) {
      ko->incrementCurrentAmount();
    }
  }
  _questBook.update(Quest::Objective::Type::KILL);
}


QuestBook& Player::getQuestBook() {
  return _questBook;
}

}  // namespace vigilante
