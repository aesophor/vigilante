// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Npc.h"

#include <json/document.h>
#include "AssetManager.h"
#include "Constants.h"
#include "item/Item.h"
#include "gameplay/ExpPointTable.h"
#include "map/GameMapManager.h"
#include "ui/dialogue/DialogueManager.h"
#include "ui/notifications/Notifications.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/CallbackUtil.h"
#include "util/RandUtil.h"
#include "util/JsonUtil.h"

#define ALLY_BODY_CATEGORY_BITS kNpc
#define ALLY_BODY_MASK_BITS kFeet | kMeleeWeapon | kCliffMarker | kProjectile
#define ALLY_FEET_MASK_BITS kGround | kPlatform | kWall | kItem | kPortal | kInteractableObject
#define ALLY_WEAPON_MASK_BITS kEnemy

#define ENEMY_BODY_CATEGORY_BITS kEnemy
#define ENEMY_BODY_MASK_BITS kFeet | kPlayer | kMeleeWeapon | kCliffMarker | kProjectile
#define ENEMY_FEET_MASK_BITS kGround | kPlatform | kWall | kItem | kInteractableObject
#define ENEMY_WEAPON_MASK_BITS kPlayer

using std::set;
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
using vigilante::category_bits::kPlayer;
using vigilante::category_bits::kEnemy;
using vigilante::category_bits::kNpc;
using vigilante::category_bits::kFeet;
using vigilante::category_bits::kMeleeWeapon;
using vigilante::category_bits::kItem;
using vigilante::category_bits::kGround;
using vigilante::category_bits::kPlatform;
using vigilante::category_bits::kCliffMarker;
using vigilante::category_bits::kWall;
using vigilante::category_bits::kPortal;
using vigilante::category_bits::kInteractableObject;
using vigilante::category_bits::kProjectile;
using rapidjson::Document;

namespace vigilante {

Npc::Npc(const string& jsonFileName)
    : Character(jsonFileName),
      _npcProfile(jsonFileName),
      _dialogueTree(_npcProfile.dialogueTreeJsonFile),
      _disposition(_npcProfile.disposition),
      _isSandboxing(_npcProfile.shouldSandbox),
      _followee(),
      _isMovingRight(),
      _moveDuration(),
      _moveTimer(),
      _waitDuration(),
      _waitTimer(),
      _calculateDistanceTimer(),
      _lastStoppedPosition() {}

void Npc::update(float delta) {
  Character::update(delta);
  act(delta);
}

void Npc::showOnMap(float x, float y) {
  if (_isShownOnMap) {
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
                        bodyCategoryBits, bodyMaskBits, feetMaskBits, weaponMaskBits);

  // Besides the original fixtures created in Character::defineBody(),
  // create one extra fixture which can collide with player's feetFixture,
  // but make it a sensor. This is the interactable area of this Npc.
  b2BodyBuilder bodyBuilder(_body);

  float scaleFactor = Director::getInstance()->getContentScaleFactor();
  b2Vec2 vertices[4];
  float sideLength = std::max(_characterProfile.bodyWidth, _characterProfile.bodyHeight) * 1.5;
  vertices[0] = {-sideLength / scaleFactor,  sideLength / scaleFactor};
  vertices[1] = { sideLength / scaleFactor,  sideLength / scaleFactor};
  vertices[2] = {-sideLength / scaleFactor, -sideLength / scaleFactor};
  vertices[3] = { sideLength / scaleFactor, -sideLength / scaleFactor};

  bodyBuilder.newPolygonFixture(vertices, 4, kPpm)
    .categoryBits(kNpc)
    .maskBits(kFeet)
    .setSensor(true)
    .setUserData(this)
    .buildFixture();
}

void Npc::import(const string& jsonFileName) {
  Character::import(jsonFileName);
  _npcProfile = Npc::Profile(jsonFileName);
}


void Npc::receiveDamage(Character* source, int damage) {
  Character::receiveDamage(source, damage);
  _isAlerted = true;


  if (!_isSetToKill) {
    return;
  }

  // Give source character exp point.
  int& sourceCharacterExp = source->getCharacterProfile().exp;
  int& sourceCharacterLevel = source->getCharacterProfile().level;

  sourceCharacterExp += getCharacterProfile().exp;
  Notifications::getInstance()->show("Acquired " + std::to_string(getCharacterProfile().exp) + " exp.");

  while (sourceCharacterExp >= exp_point_table::getNextLevelExp(sourceCharacterLevel)) {
    sourceCharacterExp -= exp_point_table::getNextLevelExp(sourceCharacterLevel);
    sourceCharacterLevel++;
    Notifications::getInstance()->show("Congratulations! You are now level " + std::to_string(sourceCharacterLevel) + ".");
  }

  // Drop items. (Here we'll use a callback to drop items
  // since creating fixtures during collision callback will crash)
  // See: https://github.com/libgdx/libgdx/issues/2730
  callback_util::runAfter([=]() {
    for (const auto& i : _npcProfile.droppedItems) {
      const string& itemJson = i.first;
      float dropChance = i.second.chance;

      float randChance = rand_util::randInt(0, 100);
      if (randChance <= dropChance) {
        float x = _body->GetPosition().x;
        float y = _body->GetPosition().y;
        int amount = rand_util::randInt(i.second.minAmount, i.second.maxAmount);
        GameMapManager::getInstance()->getGameMap()->spawnItem(itemJson, x * kPpm, y * kPpm, amount);
      }
    }
  }, .2f);
}


void Npc::onInteract(Character* user) {
  updateDialogueTreeIfNeeded();
  beginDialogue();
}

bool Npc::willInteractOnContact() const {
  return false;
}

void Npc::beginDialogue() {
  if (_npcProfile.dialogueTreeJsonFile.empty()) {
    return;
  }

  auto dialogueMgr = DialogueManager::getInstance();
  dialogueMgr->setTargetNpc(this);
  for (const auto& line : _dialogueTree.getCurrentNode()->getLines()) {
    dialogueMgr->getSubtitles()->addSubtitle(line);
  }
  dialogueMgr->getSubtitles()->beginSubtitles();
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
  _dialogueTree = DialogueTree(latestDialogueTreeJsonFileName);
}


void Npc::act(float delta) {
  if (_isKilled || _isSetToKill || _isAttacking) {
    return;
  }

  // This Npc may perform one of the following actions:
  // (1) Has `_lockedOnTarget`:
  //     a. target is within attack range -> attack()
  //     b. target not within attack range -> moveToTarget()
  // (2) Is following another Character -> moveToTarget()
  // (3) Sandboxing (just moving around wasting its time) -> moveRandomly()

  if (_lockedOnTarget && !_lockedOnTarget->isSetToKill()) {

    if (!_inRangeTargets.empty()) {  // target is within attack range
      attack();
      if (_inRangeTargets.empty()) {
        setLockedOnTarget(nullptr);
      }
    } else {  // target not within attack range
      moveToTarget(delta, _lockedOnTarget, .25f);
    }

  } else if (_followee) {
    moveToTarget(delta, _followee, .5f);
  } else if (_isSandboxing) {
    moveRandomly(delta, 0, 5, 0, 5);
  }
}

void Npc::moveToTarget(float delta, Character* target, float followDistance) {
  const b2Vec2& thisPos = _body->GetPosition();
  const b2Vec2& targetPos = target->getBody()->GetPosition();
  
  // If this character is already close enough to the target character,
  // we could return at once.
  if (std::abs(thisPos.x - targetPos.x) <= followDistance) {
    return;
  }

  (thisPos.x > targetPos.x) ? moveLeft() : moveRight();
  jumpIfStucked(delta, /*checkInterval=*/.1f);
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

Character* Npc::getFollowee() const {
  return _followee;
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

void Npc::setFollowee(Character* followee) {
  _followee = followee;
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
  shouldSandbox = json["shouldSandbox"].GetBool();
}

}  // namespace vigilante
