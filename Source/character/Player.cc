// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Player.h"

#include <string>
#include <vector>

#include "Assets.h"
#include "Audio.h"
#include "CallbackManager.h"
#include "Constants.h"
#include "character/Party.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "skill/Skill.h"
#include "skill/BackDash.h"
#include "skill/ForwardSlash.h"
#include "skill/MagicalMissile.h"
#include "quest/KillTargetObjective.h"
#include "quest/InteractWithTargetObjective.h"
#include "util/CameraUtil.h"
#include "util/StringUtil.h"

using namespace std;
using namespace vigilante::category_bits;
USING_NS_AX;

namespace vigilante {

namespace {

constexpr auto kPlayerBodyCategoryBits = kPlayer;
constexpr auto kPlayerBodyMaskBits = kFeet | kEnemy | kMeleeWeapon | kProjectile;
constexpr auto kPlayerFeetMaskBits = kGround | kPlatform | kWall | kItem | kNpc | kPortal | kInteractable;
constexpr auto kPlayerWeaponMaskBits = kEnemy;

}  // namespace

Player::Player(const std::string& jsonFileName)
    : Character{jsonFileName},
      _playerController{*this} {
  // The player has a party (team) with no other members by default.
  _party = std::make_shared<Party>(this);
}

bool Player::showOnMap(float x, float y) {
  if (_isShownOnMap || _isKilled) {
    return false;
  }

  _isShownOnMap = true;

  defineBody(b2BodyType::b2_dynamicBody, x, y,
             kPlayerBodyCategoryBits,
             kPlayerBodyMaskBits,
             kPlayerFeetMaskBits,
             kPlayerWeaponMaskBits);

  defineTexture(_characterProfile.textureResDir, x, y);

  _node->removeAllChildren();
  _node->addChild(_bodySpritesheet, graphical_layers::kPlayerBody);

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  gmMgr->getLayer()->addChild(_node, graphical_layers::kPlayerBody);

  return true;
}

void Player::onKilled() {
  Character::onKilled();

  SceneManager::the().getCurrentScene<GameScene>()->setRunning(false);
}

bool Player::inflictDamage(Character* target, int damage) {
  if (!Character::inflictDamage(target, damage)) {
    VGLOG(LOG_ERR, "Failed to inflict damage to target: [%p].", target);
    return false;
  }

  if (target->isSetToKill()) {
    auto notifications = SceneManager::the().getCurrentScene<GameScene>()->getNotifications();
    notifications->show(string_util::format("Acquired %d exp", target->getCharacterProfile().exp));

    updateKillTargetObjectives(target);
  }

  Audio::the().playSfx("Sfx/combat/sword_hit.mp3");
  camera_util::shake(4, .1f);
  return true;
}

bool Player::receiveDamage(Character* source, int damage) {
  constexpr float kNumSecCantMove = 5.0f;
  if (!Character::receiveDamage(source, damage, kNumSecCantMove)) {
    VGLOG(LOG_ERR, "Failed to receive damage from source: [%p].", source);
    return false;
  }

  _isInvincible = true;
  CallbackManager::the().runAfter([this](const CallbackManager::CallbackId){
    _isInvincible = false;
  }, 1.0f);

  auto hud = SceneManager::the().getCurrentScene<GameScene>()->getHud();
  hud->updateStatusBars();

  camera_util::shake(8, .1f);
  return true;
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

void Player::interact(Interactable* target) {
  Character::interact(target);

  if (auto targetCharacter = dynamic_cast<Character*>(target)) {
    for (auto quest : _questBook.getInProgressQuests()) {
      auto to = dynamic_cast<InteractWithTargetObjective*>(quest->getCurrentStage().objective.get());
      if (to && to->getTargetProfileJsonFileName() == targetCharacter->getCharacterProfile().jsonFileName) {
        to->setCompleted(true);
      }
    }
    _questBook.update(Quest::Objective::Type::INTERACT_WITH);
  }
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
  _playerController.handleInput();
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
