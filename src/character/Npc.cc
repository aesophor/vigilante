// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Npc.h"

#include <cmath>
#include <limits>
#include <memory>

#include <json/document.h>

#include "Assets.h"
#include "CallbackManager.h"
#include "Constants.h"
#include "character/Player.h"
#include "item/Item.h"
#include "quest/KillTargetObjective.h"
#include "quest/CollectItemObjective.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "ui/trade/TradeWindow.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/RandUtil.h"
#include "util/JsonUtil.h"
#include "util/StringUtil.h"

#define ALLY_BODY_CATEGORY_BITS kNpc
#define ALLY_BODY_MASK_BITS kFeet | kEnemy | kMeleeWeapon | kPivotMarker | kCliffMarker | kProjectile
#define ALLY_FEET_MASK_BITS kGround | kPlatform | kWall | kItem | kPortal | kInteractable
#define ALLY_WEAPON_MASK_BITS kEnemy

#define ENEMY_BODY_CATEGORY_BITS kEnemy
#define ENEMY_BODY_MASK_BITS kFeet | kPlayer | kNpc | kMeleeWeapon | kPivotMarker | kCliffMarker | kProjectile
#define ENEMY_FEET_MASK_BITS kGround | kPlatform | kWall | kItem | kInteractable
#define ENEMY_WEAPON_MASK_BITS kPlayer | kNpc

using namespace std;
using namespace vigilante::category_bits;
using rapidjson::Document;
USING_NS_CC;

namespace vigilante {

Npc::Npc(const string& jsonFileName)
    : Character(jsonFileName),
      _npcProfile(jsonFileName),
      _dialogueTree(_npcProfile.dialogueTreeJsonFile, this),
      _disposition(_npcProfile.disposition),
      _isSandboxing(_npcProfile.shouldSandbox),
      _hintBubbleFxSprite(),
      _moveDest(0.0f, 0.0f),
      _isMovingRight(),
      _moveDuration(),
      _moveTimer(),
      _waitDuration(),
      _waitTimer(),
      _calculateDistanceTimer(),
      _lastStoppedPosition() {
  if (_npcProfile.isUnsheathed) {
    unsheathWeapon();
  }
}

void Npc::update(float delta) {
  Character::update(delta);

  if (!_isShownOnMap || _isKilled) {
    return;
  }

  // Sync the hint bubble fx sprite with Npc's b2body if it exists.
  if (_hintBubbleFxSprite) {
    const b2Vec2& b2bodyPos = _body->GetPosition();
    _hintBubbleFxSprite->setPosition(b2bodyPos.x * kPpm,
                                     b2bodyPos.y * kPpm + HINT_BUBBLE_FX_SPRITE_OFFSET_Y);
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  if (gmMgr->areNpcsAllowedToAct()) {
    act(delta);
  }
}

bool Npc::showOnMap(float x, float y) {
  if (_isShownOnMap || _isKilled) {
    return false;
  }

  _isShownOnMap = true;

  // Construct b2Body and b2Fixtures.
  // The category/mask bits of each fixture are set in Npc::setDisposition()
  // based on the disposition of this npc.
  defineBody(b2BodyType::b2_dynamicBody, x, y);
  setDisposition(_disposition);

  // Load sprites, spritesheets, and animations, and then add them to GameMapManager layer.
  defineTexture(_characterProfile.textureResDir, x, y);

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  gmMgr->getLayer()->addChild(_bodySpritesheet, graphical_layers::kNpcBody);
  for (auto equipment : _equipmentSlots) {
    if (equipment) {
      Equipment::Type type = equipment->getEquipmentProfile().equipmentType;
      gmMgr->getLayer()->addChild(_equipmentSpritesheets[type], graphical_layers::kEquipment - type);
    }
  }

  return true;
}

void Npc::defineBody(b2BodyType bodyType, float x, float y,
                     short bodyCategoryBits, short bodyMaskBits,
                     short feetMaskBits, short weaponMaskBits) {
  Character::defineBody(bodyType, x, y,
                        bodyCategoryBits,
                        bodyMaskBits,
                        feetMaskBits,
                        weaponMaskBits);

  // Besides the original fixtures created in Character::defineBody(),
  // create one extra fixture which can collide with player's feetFixture,
  // but make it a sensor. This is the interactable area of this Npc.
  b2BodyBuilder bodyBuilder(_body);

  float scaleFactor = Director::getInstance()->getContentScaleFactor();
  b2Vec2 vertices[4];
  float sideLength = std::max(_characterProfile.bodyWidth, _characterProfile.bodyHeight) * 1.2;
  vertices[0] = {-sideLength / scaleFactor,  sideLength / scaleFactor};
  vertices[1] = { sideLength / scaleFactor,  sideLength / scaleFactor};
  vertices[2] = {-sideLength / scaleFactor, -sideLength / scaleFactor};
  vertices[3] = { sideLength / scaleFactor, -sideLength / scaleFactor};

  bodyBuilder.newPolygonFixture(vertices, 4, kPpm)
    .categoryBits(kInteractable)
    .maskBits(kFeet)
    .setSensor(true)
    .setUserData(static_cast<Interactable*>(this))
    .buildFixture();
}

void Npc::import(const string& jsonFileName) {
  Character::import(jsonFileName);
  _npcProfile = Npc::Profile(jsonFileName);
}

void Npc::onKilled() {
  Character::onKilled();
  dropItems();

  if (!_npcProfile.isRespawnable) {
    auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
    gmMgr->setNpcAllowedToSpawn(_characterProfile.jsonFileName, false);
  }
}

void Npc::onMapChanged() {
  clearMoveDest();

  if (_isKilled && _party) {
    _party->dismiss(this, /*addToMap=*/false);
  }
}

void Npc::inflictDamage(Character* target, int damage) {
  Character::inflictDamage(target, damage);

  // If this Npc is in the player's party, and this Npc kills an enemy,
  // then update player's quests with KillTargetObjective.
  if (_disposition == Npc::Disposition::ALLY && target->isSetToKill() && _party) {
    auto player = dynamic_cast<Player*>(_party->getLeader());
    if (player) {
      player->updateKillTargetObjectives(target);
    }
  }
}

void Npc::receiveDamage(Character* source, int damage) {
  Character::receiveDamage(source, damage);
  _isAlerted = true;

  if (!_isSetToKill) {
    return;
  }

  source->addExp(_characterProfile.exp);
}

void Npc::interact(Interactable* target) {
  if (!dynamic_cast<GameMap::Portal*>(target)) {
    Character::interact(target);
  }
}

void Npc::onInteract(Character*) {
  updateDialogueTreeIfNeeded();
  beginDialogue();
}

bool Npc::willInteractOnContact() const {
  return false;
}

void Npc::showHintUI() {
  if (_npcProfile.dialogueTreeJsonFile.empty()) {
    return;
  }

  createHintBubbleFx();
  
  auto controlHints = SceneManager::the().getCurrentScene<GameScene>()->getControlHints();
  controlHints->insert({EventKeyboard::KeyCode::KEY_CAPITAL_E}, "Talk");
}

void Npc::hideHintUI() {
  removeHintBubbleFx();
  
  auto controlHints = SceneManager::the().getCurrentScene<GameScene>()->getControlHints();
  controlHints->remove({EventKeyboard::KeyCode::KEY_CAPITAL_E});
}

void Npc::createHintBubbleFx() {
  if (_hintBubbleFxSprite) {
    return;
  }

  auto fxMgr = SceneManager::the().getCurrentScene<GameScene>()->getFxManager();
  _hintBubbleFxSprite = fxMgr->createHintBubbleFx(_body, "dialogue_available");
}

void Npc::removeHintBubbleFx() {
  if (!_hintBubbleFxSprite) {
    return;
  }

  auto fxMgr = SceneManager::the().getCurrentScene<GameScene>()->getFxManager();
  fxMgr->removeFx(_hintBubbleFxSprite);
  _hintBubbleFxSprite = nullptr;
}

void Npc::dropItems() {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();

  // We'll use a callback to drop items since creating fixtures during collision callback
  // will cause the game to crash. Ref: https://github.com/libgdx/libgdx/issues/2730
  CallbackManager::the().runAfter([=]() {
    for (const auto& i : _npcProfile.droppedItems) {
      const string& itemJson = i.first;
      float dropChance = i.second.chance;

      float randChance = rand_util::randInt(0, 100);
      if (randChance <= dropChance) {
        float x = _body->GetPosition().x;
        float y = _body->GetPosition().y;
        int amount = rand_util::randInt(i.second.minAmount, i.second.maxAmount);
        gmMgr->getGameMap()->createItem(itemJson, x * kPpm, y * kPpm, amount);
      }
    }
  }, .2f);
}

void Npc::updateDialogueTreeIfNeeded() {
  auto dialogueMgr = SceneManager::the().getCurrentScene<GameScene>()->getDialogueManager();
  string latestDialogueTreeJsonFileName
    = dialogueMgr->getLatestNpcDialogueTree(_characterProfile.jsonFileName);
  if (latestDialogueTreeJsonFileName.empty()) {
    return;
  }

  VGLOG(LOG_INFO, "Loading %s's dialogue tree: %s", _characterProfile.jsonFileName.c_str(),
                                                    latestDialogueTreeJsonFileName.c_str());
  _dialogueTree = DialogueTree(latestDialogueTreeJsonFileName, this);
}

void Npc::beginDialogue() {
  if (_npcProfile.dialogueTreeJsonFile.empty()) {
    return;
  }

  onDialogueBegin();

  auto dialogueMgr = SceneManager::the().getCurrentScene<GameScene>()->getDialogueManager();
  dialogueMgr->setTargetNpc(this);
  for (const auto& line : _dialogueTree.getCurrentNode()->getLines()) {
    dialogueMgr->getSubtitles()->addSubtitle(line);
  }
  dialogueMgr->getSubtitles()->beginSubtitles();
}

void Npc::beginTrade() {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  auto wm = SceneManager::the().getCurrentScene<GameScene>()->getWindowManager();

  wm->push(std::make_unique<TradeWindow>(/*buyer=*/gmMgr->getPlayer(), /*seller=*/this));
}

void Npc::onDialogueBegin() {
  auto controlHints = SceneManager::the().getCurrentScene<GameScene>()->getControlHints();
  controlHints->setVisible(false);
}

void Npc::onDialogueEnd() {
  auto controlHints = SceneManager::the().getCurrentScene<GameScene>()->getControlHints();
  controlHints->setVisible(true);
}

// This Npc may perform one of the following actions:
// (1) Has `_lockedOnTarget` and `_lockedOnTarget` is not dead yet:
//     a. target is within attack range -> attack()
//     b. target not within attack range -> moveToTarget()
// (2) Has `_lockedOnTarget` but `_lockedOnTarget` is dead:
//     a. target belongs to a party -> try to select other member as new _lockedOnTarget
//     b. target doesnt belong to any party -> clear _lockedOnTarget
// (3) If too far away from the party leader, teleport to the leader.
// (4) Has a target destination (_moveDest) to travel to
// (5) Is following another Character -> moveToTarget()
// (6) Sandboxing (just moving around wasting its time) -> moveRandomly()
void Npc::act(float delta) {
  if (_isKilled || _isSetToKill || _isAttacking) {
    return;
  }

  if (_lockedOnTarget && !_lockedOnTarget->isSetToKill()) {
    if (!_inRangeTargets.empty()) {
      attack();
    } else {
      moveToTarget(delta, _lockedOnTarget, _characterProfile.attackRange / kPpm);
    }
  } else if (_lockedOnTarget && _lockedOnTarget->isSetToKill()) {
    Character* killedTarget = _lockedOnTarget;
    setLockedOnTarget(nullptr);
    findNewLockedOnTargetFromParty(killedTarget);
  } else if (_party && isTooFarAwayFromTarget(_party->getLeader())) {
    clearMoveDest();
    teleportToTarget(_party->getLeader());
  } else if (_moveDest.x || _moveDest.y) {
    moveToTarget(delta, _moveDest, Npc::_kMoveDestFollowDist);
  } else if (_party && !isWaitingForPartyLeader()) {
    moveToTarget(delta, _party->getLeader(), Npc::_kAllyFollowDist);
  } else if (_isSandboxing) {
    moveRandomly(delta, 0, 5, 0, 5);
  }
}

void Npc::findNewLockedOnTargetFromParty(Character* killedTarget) {
  if (!killedTarget->getParty()) {
    return;
  }

  for (auto member : killedTarget->getParty()->getLeaderAndMembers()) {
    if (!member->isSetToKill()) {
      setLockedOnTarget(member);
      return;
    }
  }
}

bool Npc::isTooFarAwayFromTarget(Character* target) const {
  const b2Vec2& thisPos = _body->GetPosition();
  const b2Vec2& targetPos = target->getBody()->GetPosition();

  return std::hypotf(targetPos.x - thisPos.x, targetPos.y - thisPos.y) > Npc::_kAllyTeleportDist;
}

void Npc::teleportToTarget(Character* target) {
  if (!target->getBody()) {
    VGLOG(LOG_WARN, "Unable to move to target: %s (b2body missing)",
                    target->getCharacterProfile().name.c_str());
    return;
  }

  teleportToTarget(target->getBody()->GetPosition());
}

void Npc::teleportToTarget(const b2Vec2& targetPos) {
  setPosition(targetPos.x, targetPos.y);
}

void Npc::moveToTarget(float delta, Character* target, float followDist) {
  if (!target->getBody()) {
    VGLOG(LOG_WARN, "Unable to move to target: %s (b2body missing)",
                    target->getCharacterProfile().name.c_str());
    return;
  }

  moveToTarget(delta, target->getBody()->GetPosition(), followDist);
}

void Npc::moveToTarget(float delta, const b2Vec2& targetPos, float followDist) {
  const b2Vec2& thisPos = _body->GetPosition();
  if (std::hypotf(targetPos.x - thisPos.x, targetPos.y - thisPos.y) <= followDist) {
    _moveDest.SetZero();
    return;
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  PathFinder* pathFinder = gmMgr->getGameMap()->getPathFinder();
  if (auto nextHop = pathFinder->findOptimalNextHop(thisPos, targetPos, followDist)) {
    _moveDest = *nextHop;
  } else if (std::abs(targetPos.x - thisPos.x) > .2f) {
    (thisPos.x > targetPos.x) ? moveLeft() : moveRight();
  }

  // Sometimes when two Npcs are too close to each other,
  // they will stuck in the same place, unable to attack each other.
  // This is most likely because they are facing at the wrong direction.
  _isFacingRight = targetPos.x > thisPos.x;

  jumpIfStucked(delta, Npc::_kJumpCheckInterval);
}

void Npc::moveRandomly(float delta,
                       int minMoveDuration, int maxMoveDuration,
                       int minWaitDuration, int maxWaitDuration) {
  // If the character has finished moving and waiting, regenerate random values for
  // _moveDuration and _waitDuration within the specified range.
  if (_moveTimer >= _moveDuration && _waitTimer >= _waitDuration) {
    _isMovingRight = static_cast<bool>(rand_util::randInt(0, 1));
    _moveDuration = rand_util::randInt(minMoveDuration, maxMoveDuration);
    _waitDuration = rand_util::randInt(minWaitDuration, maxWaitDuration);
    _moveTimer = 0;
    _waitTimer = 0;
  }

  if (_moveTimer >= _moveDuration) {
    _waitTimer += delta;
  } else {
    _moveTimer += delta;
    (_isMovingRight) ? moveRight() : moveLeft();
    jumpIfStucked(delta, /*checkInterval=*/.5f);
  }
}

void Npc::jumpIfStucked(float delta, float checkInterval) {
  // If we haven't reached checkInterval yet, add delta to the timer
  // and return at once.
  if (_calculateDistanceTimer <= checkInterval) {
    _calculateDistanceTimer += delta;
    return;
  }

  // We've reached checkInterval, so we can make this character jump
  // if it hasn't moved at all, and then reset the timer.
  if (std::abs(_body->GetPosition().x - _lastStoppedPosition.x) == 0) {
    jump();
  }
  _lastStoppedPosition = {_body->GetPosition().x, _body->GetPosition().y};
  _calculateDistanceTimer = 0;
}

void Npc::reverseDirection() {
  _isMovingRight = !_isMovingRight;
}

bool Npc::isPlayerLeaderOfParty() const {
  return _party ? dynamic_cast<Player*>(_party->getLeader()) != nullptr : false;
}

bool Npc::isWaitingForPlayer() const {
  if (!_party) {
    return false;
  }

  return isPlayerLeaderOfParty() &&
    _party->hasWaitingMember(_characterProfile.jsonFileName);
}

bool Npc::isWaitingForPartyLeader() const {
  if (!_party) {
    return false;
  }

  return _party->hasWaitingMember(_characterProfile.jsonFileName);
}

void Npc::setDisposition(Npc::Disposition disposition) {
  _disposition = disposition;

  switch (disposition) {
    case Npc::Disposition::ALLY:
      DynamicActor::setCategoryBits(_fixtures[FixtureType::BODY], ALLY_BODY_CATEGORY_BITS);
      DynamicActor::setMaskBits(_fixtures[FixtureType::BODY], ALLY_BODY_MASK_BITS);
      DynamicActor::setMaskBits(_fixtures[FixtureType::FEET], ALLY_FEET_MASK_BITS);
      DynamicActor::setMaskBits(_fixtures[FixtureType::WEAPON], ALLY_WEAPON_MASK_BITS);
      break;
    case Npc::Disposition::ENEMY:
      DynamicActor::setCategoryBits(_fixtures[FixtureType::BODY], ENEMY_BODY_CATEGORY_BITS);
      DynamicActor::setMaskBits(_fixtures[FixtureType::BODY], ENEMY_BODY_MASK_BITS);
      DynamicActor::setMaskBits(_fixtures[FixtureType::FEET], ENEMY_FEET_MASK_BITS);
      DynamicActor::setMaskBits(_fixtures[FixtureType::WEAPON], ENEMY_WEAPON_MASK_BITS);
      break;
    default:
      VGLOG(LOG_ERR, "Invalid disposition for user: %s", _characterProfile.name.c_str());
      break;
  }
}

Npc::Profile::Profile(const string& jsonFileName) {
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

  dialogueTreeJsonFile = json["dialogueTree"].GetString();
  disposition = static_cast<Npc::Disposition>(json["disposition"].GetInt());
  isRespawnable = json["isRespawnable"].GetBool();
  isRecruitable = json["isRecruitable"].GetBool();
  isUnsheathed = json["isUnsheathed"].GetBool();
  isTradable = json["isTradable"].GetBool();
  shouldSandbox = json["shouldSandbox"].GetBool();
}

}  // namespace vigilante
