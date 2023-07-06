// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Player.h"

#include <string>
#include <vector>

#include "Assets.h"
#include "Audio.h"
#include "CallbackManager.h"
#include "Constants.h"
#include "character/Party.h"
#include "input/Keybindable.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "skill/Skill.h"
#include "skill/BackDash.h"
#include "skill/ForwardSlash.h"
#include "skill/MagicalMissile.h"
#include "quest/KillTargetObjective.h"
#include "util/CameraUtil.h"
#include "util/StringUtil.h"

#define PLAYER_BODY_CATEGORY_BITS kPlayer
#define PLAYER_BODY_MASK_BITS kFeet | kEnemy | kMeleeWeapon | kProjectile
#define PLAYER_FEET_MASK_BITS kGround | kPlatform | kWall | kItem | kNpc | kPortal | kInteractable
#define PLAYER_WEAPON_MASK_BITS kEnemy

using namespace std;
using namespace vigilante::category_bits;
USING_NS_AX;

namespace vigilante {

Player::Player(const std::string& jsonFileName)
    : Character(jsonFileName),
      _questBook(assets::kQuestsList) {
  // The player has a party (team) with no other members by default.
  _party = std::make_shared<Party>(this);
}

bool Player::showOnMap(float x, float y) {
  if (_isShownOnMap || _isKilled) {
    return false;
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

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  gmMgr->getLayer()->addChild(_bodySpritesheet, graphical_layers::kPlayerBody);
  for (auto equipment : _equipmentSlots) {
    if (equipment) {
      Equipment::Type type = equipment->getEquipmentProfile().equipmentType;
      gmMgr->getLayer()->addChild(_equipmentSpritesheets[type], graphical_layers::kEquipment - type);
    }
  }

  return true;
}

void Player::attack() {
  Character::attack();

  Audio::the().playSfx("Sfx/combat/sword_attack.mp3");
}

void Player::onKilled() {
  Character::onKilled();

  SceneManager::the().getCurrentScene<GameScene>()->setRunning(false);
}

void Player::inflictDamage(Character* target, int damage) {
  Character::inflictDamage(target, damage);
  camera_util::shake(8, .1f);

  if (target->isSetToKill()) {
    auto notifications = SceneManager::the().getCurrentScene<GameScene>()->getNotifications();
    notifications->show(string_util::format("Acquired %d exp", target->getCharacterProfile().exp));

    updateKillTargetObjectives(target);
  }

  Audio::the().playSfx("Sfx/combat/sword_hit.mp3");
}

void Player::receiveDamage(Character* source, int damage) {
  Character::receiveDamage(source, damage);
  camera_util::shake(8, .1f);

  _fixtures[FixtureType::BODY]->SetSensor(true);
  _isInvincible = true;

  CallbackManager::the().runAfter([&](){
    _fixtures[FixtureType::BODY]->SetSensor(false);
    _isInvincible = false;
  }, 1.0f);

  auto hud = SceneManager::the().getCurrentScene<GameScene>()->getHud();
  hud->updateStatusBars();
}

void Player::addItem(shared_ptr<Item> item, int amount) {
  Character::addItem(item, amount);

  auto notifications = SceneManager::the().getCurrentScene<GameScene>()->getNotifications();
  notifications->show((amount > 1) ?
      string_util::format("Acquired item: %s (%d).", item->getName().c_str(), amount) :
      string_util::format("Acquired item: %s.", item->getName().c_str()));
}

void Player::removeItem(Item* item, int amount) {
  const string itemName = item->getName();
  Character::removeItem(item, amount);

  auto notifications = SceneManager::the().getCurrentScene<GameScene>()->getNotifications();
  notifications->show((amount > 1) ?
      string_util::format("Removed item: %s (%d).", itemName.c_str(), amount) :
      string_util::format("Removed item: %s.", itemName.c_str()));
}

void Player::equip(Equipment* equipment, bool audio) {
  Character::equip(equipment, audio);

  auto hud = SceneManager::the().getCurrentScene<GameScene>()->getHud();
  hud->updateEquippedWeapon();
}

void Player::unequip(Equipment::Type equipmentType, bool audio) {
  Character::unequip(equipmentType, audio);

  auto hud = SceneManager::the().getCurrentScene<GameScene>()->getHud();
  hud->updateEquippedWeapon();
}

void Player::pickupItem(Item* item) {
  Character::pickupItem(item);
  _questBook.update(Quest::Objective::Type::COLLECT);
}

void Player::addExp(const int exp) {
  int originalLevel = _characterProfile.level;
  Character::addExp(exp);

  if (_characterProfile.level > originalLevel) {
    auto notifications = SceneManager::the().getCurrentScene<GameScene>()->getNotifications();
    notifications->show(string_util::format("Congrats! You are now level %d.", _characterProfile.level));
  }
}

void Player::handleInput() {
  if (_isSetToKill || _isAttacking || _isUsingSkill || _isSheathingWeapon || _isUnsheathingWeapon) {
    return;
  }

  if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_E)) {
    if (_inRangeInteractables.size()) {
      interact(*_inRangeInteractables.begin());
    }
    return;
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_UP_ARROW)) {
    if (_portal) {
      interact(_portal);
    }
    return;
  }

  if (IS_KEY_PRESSED(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
    crouch();
    if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_LEFT_ALT)) {
      jumpDown();
    }
  }

  if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_LEFT_CTRL)) {
    if (!_isWeaponSheathed) {
      attack();
    }
  }

  if (IS_KEY_PRESSED(EventKeyboard::KeyCode::KEY_LEFT_ARROW)) {
    moveLeft();
  } else if (IS_KEY_PRESSED(EventKeyboard::KeyCode::KEY_RIGHT_ARROW)) {
    moveRight();
  }

  if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_R)) {
    if (_equipmentSlots[Equipment::Type::WEAPON]
        && _isWeaponSheathed && !_isUnsheathingWeapon) {
      unsheathWeapon();
    } else if (!_isWeaponSheathed && !_isSheathingWeapon) {
      sheathWeapon();
    }
  }

  auto hotkeyMgr = SceneManager::the().getCurrentScene<GameScene>()->getHotkeyManager();
  for (auto keyCode : HotkeyManager::_kBindableKeys) {
    Keybindable* action = hotkeyMgr->getHotkeyAction(keyCode);
    if (IS_KEY_JUST_PRESSED(keyCode) && action) {
      if (dynamic_cast<Skill*>(action)) {
        activateSkill(dynamic_cast<Skill*>(action));
      } else if (dynamic_cast<Consumable*>(action)) {
        useItem(dynamic_cast<Consumable*>(action));
      }
    }
  }

  if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_Z)) {
    if (!_inRangeItems.empty()) {
      pickupItem(*_inRangeItems.begin());
    }
  }

  if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_ALT)) {
    jump();
  }

  if (_isCrouching && !IS_KEY_PRESSED(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
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

}  // namespace vigilante
