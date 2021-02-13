// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Npc.h"

#include <memory>

#include <json/document.h>
#include "std/make_unique.h"
#include "AssetManager.h"
#include "CallbackManager.h"
#include "Constants.h"
#include "character/Player.h"
#include "item/Item.h"
#include "map/GameMapManager.h"
#include "map/FxManager.h"
#include "quest/KillTargetObjective.h"
#include "quest/CollectItemObjective.h"
#include "ui/WindowManager.h"
#include "ui/control_hints/ControlHints.h"
#include "ui/dialogue/DialogueManager.h"
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

#define ALLY_FOLLOW_DISTANCE .75f

using std::set;
using std::string;
using std::vector;
using std::unique_ptr;
using std::unordered_set;
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
using cocos2d::EventKeyboard;
using vigilante::category_bits::kPlayer;
using vigilante::category_bits::kEnemy;
using vigilante::category_bits::kNpc;
using vigilante::category_bits::kFeet;
using vigilante::category_bits::kMeleeWeapon;
using vigilante::category_bits::kItem;
using vigilante::category_bits::kGround;
using vigilante::category_bits::kPlatform;
using vigilante::category_bits::kPivotMarker;
using vigilante::category_bits::kCliffMarker;
using vigilante::category_bits::kWall;
using vigilante::category_bits::kPortal;
using vigilante::category_bits::kInteractable;
using vigilante::category_bits::kProjectile;
using rapidjson::Document;

namespace vigilante {

bool Npc::_areNpcsAllowedToAct = true;
unordered_set<string> Npc::_npcSpawningBlacklist;

Npc::Npc(const string& jsonFileName)
    : Character(jsonFileName),
      _npcProfile(jsonFileName),
      _dialogueTree(_npcProfile.dialogueTreeJsonFile, this),
      _disposition(_npcProfile.disposition),
      _isSandboxing(_npcProfile.shouldSandbox),
      _hintBubbleFxSprite(),
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

  if (_areNpcsAllowedToAct) {
    act(delta);
  }
}

void Npc::showOnMap(float x, float y) {
  if (_isShownOnMap || _isKilled) {
    return;
  }
  _isShownOnMap = true;

  // Construct b2Body and b2Fixtures.
  // The category/mask bits of each fixture are set in Npc::setDisposition()
  // based on the disposition of this npc.
  defineBody(b2BodyType::b2_dynamicBody, x, y);
  setDisposition(_disposition);

  // Load sprites, spritesheets, and animations, and then add them to GameMapManager layer.
  defineTexture(_characterProfile.textureResDir, x, y);
  GameMapManager* gmMgr = GameMapManager::getInstance();
  gmMgr->getLayer()->addChild(_bodySpritesheet, graphical_layers::kNpcBody);
  for (auto equipment : _equipmentSlots) {
    if (equipment) {
      Equipment::Type type = equipment->getEquipmentProfile().equipmentType;
      gmMgr->getLayer()->addChild(_equipmentSpritesheets[type], graphical_layers::kEquipment - type);
    }
  }
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

  if (!_npcProfile.isRespawnable) {
    Npc::setNpcAllowedToSpawn(_characterProfile.jsonFileName, false);
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

  // Give exp point to source character.
  source->addExp(_characterProfile.exp);
 
  // Drop items. (Here we'll use a callback to drop items
  // since creating fixtures during collision callback will crash)
  // See: https://github.com/libgdx/libgdx/issues/2730
  CallbackManager::getInstance()->runAfter([=]() {
    for (const auto& i : _npcProfile.droppedItems) {
      const string& itemJson = i.first;
      float dropChance = i.second.chance;

      float randChance = rand_util::randInt(0, 100);
      if (randChance <= dropChance) {
        float x = _body->GetPosition().x;
        float y = _body->GetPosition().y;
        int amount = rand_util::randInt(i.second.minAmount, i.second.maxAmount);
        GameMapManager::getInstance()->getGameMap()->createItem(itemJson, x * kPpm, y * kPpm, amount);
      }
    }
  }, .2f);
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
  createHintBubbleFx();
  ControlHints::getInstance()->show(EventKeyboard::KeyCode::KEY_CAPITAL_E, "Talk");
}

void Npc::hideHintUI() {
  removeHintBubbleFx();
  ControlHints::getInstance()->hide(EventKeyboard::KeyCode::KEY_CAPITAL_E);
}

void Npc::createHintBubbleFx() {
  if (_hintBubbleFxSprite) {
    return;
  }

  const b2Vec2& bodyPos = _body->GetPosition();
  float x = bodyPos.x * kPpm;
  float y = bodyPos.y * kPpm + HINT_BUBBLE_FX_SPRITE_OFFSET_Y;

  _hintBubbleFxSprite
    = GameMapManager::getInstance()->getFxManager()->createFx(
        "Texture/fx/hint_bubble", "dialogue_available", x, y, -1, 45.0f);
}

void Npc::removeHintBubbleFx() {
  if (!_hintBubbleFxSprite) {
    return;
  }

  _hintBubbleFxSprite->stopAllActions();
  _hintBubbleFxSprite->removeFromParent();
  _hintBubbleFxSprite = nullptr;
}


void Npc::beginDialogue() {
  if (_npcProfile.dialogueTreeJsonFile.empty()) {
    return;
  }

  removeHintBubbleFx();

  auto dialogueMgr = DialogueManager::getInstance();
  dialogueMgr->setTargetNpc(this);
  for (const auto& line : _dialogueTree.getCurrentNode()->getLines()) {
    dialogueMgr->getSubtitles()->addSubtitle(line);
  }
  dialogueMgr->getSubtitles()->beginSubtitles();
}

void Npc::beginTrade() {
  auto player = GameMapManager::getInstance()->getPlayer();

  WindowManager::getInstance()->push(
      std::make_unique<TradeWindow>(/*buyer=*/player, /*seller=*/this));
}


void Npc::updateDialogueTreeIfNeeded() {
  // Fetch the latest update from DialogueTree::_latestNpcDialogueTree.
  // See gameplay/DialogueTree.cc
  string latestDialogueTreeJsonFileName
    = DialogueTree::getLatestNpcDialogueTree(_characterProfile.jsonFileName);

  if (latestDialogueTreeJsonFileName.empty()) {
    return;
  }

  VGLOG(LOG_INFO, "Loading %s's dialogue tree: %s", _characterProfile.jsonFileName.c_str(),
                                                    latestDialogueTreeJsonFileName.c_str());
  _dialogueTree = DialogueTree(latestDialogueTreeJsonFileName, this);
}


void Npc::act(float delta) {
  if (_isKilled || _isSetToKill || _isAttacking) {
    return;
  }


  // This Npc may perform one of the following actions:
  // (1) Has `_lockedOnTarget` and `_lockedOnTarget` is not dead yet:
  //     a. target is within attack range -> attack()
  //     b. target not within attack range -> moveToTarget()
  // (2) Has `_lockedOnTarget` but `_lockedOnTarget` is dead:
  //     a. target belongs to a party -> try to select other member as new _lockedOnTarget
  //     b. target doesnt belong to any party -> clear _lockedOnTarget
  // (3) Is following another Character -> moveToTarget()
  // (4) Sandboxing (just moving around wasting its time) -> moveRandomly()

  if (_lockedOnTarget && !_lockedOnTarget->isSetToKill()) {

    if (!_inRangeTargets.empty()) {  // target is within attack range
      attack();
    } else {  // target not within attack range
      moveToTarget(delta, _lockedOnTarget, _characterProfile.attackRange / kPpm);
    }

  } else if (_lockedOnTarget && _lockedOnTarget->isSetToKill()) {
    Character* killedTarget = _lockedOnTarget;
    setLockedOnTarget(nullptr);
    findNewLockedOnTargetFromParty(killedTarget);
  } else if (_party && !isWaitingForPlayer()) {
    moveToTarget(delta, _party->getLeader(), ALLY_FOLLOW_DISTANCE);
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

void Npc::moveToTarget(float delta, Character* target, float followDistance) {
  assert(_body != nullptr);

  if (!target->getBody()) {
    VGLOG(LOG_WARN, "Unable to move to target: %s (b2body missing)",
                    target->getCharacterProfile().name.c_str());
    return;
  }

  const b2Vec2& thisPos = _body->GetPosition();
  const b2Vec2& targetPos = target->getBody()->GetPosition();
  
  // If this character is already close enough to the target character,
  // we could return at once.
  if (std::abs(thisPos.x - targetPos.x) <= followDistance) {
    return;
  }

  // Sometimes when Npcs are too close to each other,
  // they will stuck in the same place, unable to attack each other.
  // This is most likely because they are facing at the wrong direction.
  _isFacingRight = targetPos.x - thisPos.x > 0;

  (thisPos.x > targetPos.x) ? moveLeft() : moveRight();
  jumpIfStucked(delta, /*checkInterval=*/.5f);
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


bool Npc::isInPlayerParty() const {
  return (_party) ? dynamic_cast<Player*>(_party->getLeader()) != nullptr : false;
}

bool Npc::isWaitingForPlayer() const {
  return isInPlayerParty() && _party->hasWaitingMember(_characterProfile.jsonFileName);
}


Npc::Profile& Npc::getNpcProfile() {
  return _npcProfile;
}

DialogueTree& Npc::getDialogueTree() {
  return _dialogueTree;
}

Npc::Disposition Npc::getDisposition() const {
  return _disposition;
}

bool Npc::isSandboxing() const {
  return _isSandboxing;
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

void Npc::setSandboxing(bool sandboxing) {
  _isSandboxing = sandboxing;
}


void Npc::setNpcsAllowedToAct(bool npcsAllowedToAct) {
  Npc::_areNpcsAllowedToAct = npcsAllowedToAct;
}

bool Npc::isNpcAllowedToSpawn(const string& jsonFileName) {
  return Npc::_npcSpawningBlacklist.find(jsonFileName)
      == Npc::_npcSpawningBlacklist.end();
}

void Npc::setNpcAllowedToSpawn(const string& jsonFileName, bool canSpawn) {
  if (!canSpawn && Npc::isNpcAllowedToSpawn(jsonFileName)) {
    Npc::_npcSpawningBlacklist.insert(jsonFileName);
  } else if (canSpawn && !Npc::isNpcAllowedToSpawn(jsonFileName)) {
    Npc::_npcSpawningBlacklist.erase(jsonFileName);
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
