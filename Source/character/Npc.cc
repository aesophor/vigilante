// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "Npc.h"

#include <cmath>
#include <limits>
#include <memory>

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
#include "util/B2BodyBuilder.h"
#include "util/JsonUtil.h"
#include "util/RandUtil.h"
#include "util/StringUtil.h"

using namespace std;
using namespace vigilante::assets;
using namespace vigilante::category_bits;
USING_NS_AX;

namespace vigilante {

namespace {

constexpr auto kAllyBodyCategoryBits = kNpc;
constexpr auto kAllyBodyMaskBits = kFeet | kEnemy | kMeleeWeapon | kPivotMarker | kCliffMarker | kProjectile;
constexpr auto kAllyFeetMaskBits = kGround | kPlatform | kWall | kItem | kPortal | kInteractable;
constexpr auto kAllyWeaponMaskBits = kEnemy;

constexpr auto kEnemyBodyCategoryBits = kEnemy;
constexpr auto kEnemyBodyMaskBits = kFeet | kPlayer | kNpc | kMeleeWeapon | kPivotMarker | kCliffMarker | kProjectile;
constexpr auto kEnemyFeetMaskBits = kGround | kPlatform | kWall | kItem | kInteractable;
constexpr auto kEnemyWeaponMaskBits = kPlayer | kNpc;

}  // namespace

Npc::Npc(const string& jsonFilePath)
    : Character{jsonFilePath},
      _npcProfile{jsonFilePath},
      _dialogueTree{_npcProfile.dialogueTreeJsonFile, this},
      _disposition{_npcProfile.disposition},
      _npcController(*this) {
  if (_npcProfile.shouldSandbox) {
    _npcController.setSandboxing(_npcProfile.shouldSandbox);
  }
}

void Npc::update(const float delta) {
  Character::update(delta);

  if (!_isShownOnMap || _isKilled) {
    return;
  }

  const b2Vec2& b2bodyPos = _body->GetPosition();

  // Sync the floating health bar with Npc's b2body if it exists.
  if (_floatingHealthBar->isVisible()) {
    const float floatingHealthBarX = b2bodyPos.x * kPpm - _floatingHealthBar->getMaxLength() / 2;
    const float floatingHealthBarY = b2bodyPos.y * kPpm + _characterProfile.bodyHeight / 2 + 10.0f;
    _floatingHealthBar->setPosition({floatingHealthBarX, floatingHealthBarY});
    _floatingHealthBar->update(delta);
  }

  // Sync the hint bubble fx sprite with Npc's b2body if it exists.
  if (_hintBubbleFxSprite) {
    const float hintBubbleX = b2bodyPos.x * kPpm;
    const float hintBubbleY = b2bodyPos.y * kPpm + kHintBubbleFxSpriteOffsetY;
    _hintBubbleFxSprite->setPosition(hintBubbleX, hintBubbleY);
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  if (gmMgr->areNpcsAllowedToAct()) {
    act(delta);
  }
}

bool Npc::showOnMap(float x, float y) {
  if (!Character::showOnMap(x, y)) {
    return false;
  }

  // Construct b2Body and b2Fixtures.
  // The category/mask bits of each fixture are set in Npc::setDisposition()
  // based on the disposition of this npc.
  defineBody(b2BodyType::b2_dynamicBody, x, y);
  setDisposition(_disposition);

  // Load sprites, spritesheets, and animations, and then add them to GameMapManager layer.
  defineTexture(_characterProfile.textureResDir, x, y);

  _floatingHealthBar = make_unique<StatusBar>(kBarLeftPadding, kBarRightPadding, kHealthBar, 45.0f, 5.0f);
  _floatingHealthBar->setVisible(false);

  _node->removeAllChildren();
  _node->addChild(_bodySpritesheet, graphical_layers::kNpcBody);
  _node->addChild(_floatingHealthBar->getLayout(), graphical_layers::kHud);

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  gmMgr->getLayer()->addChild(_node, graphical_layers::kNpcBody);

  return true;
}

bool Npc::removeFromMap() {
  if (!Character::removeFromMap()) {
    return false;
  }

  _floatingHealthBar->getLayout()->removeFromParentAndCleanup(true);
}

void Npc::defineBody(b2BodyType bodyType, float x, float y,
                     short bodyCategoryBits, short bodyMaskBits,
                     short feetMaskBits, short weaponMaskBits) {
  Character::defineBody(bodyType, x, y,
                        bodyCategoryBits,
                        bodyMaskBits,
                        feetMaskBits,
                        weaponMaskBits);

  float scaleFactor = Director::getInstance()->getContentScaleFactor();
  float sideLength = std::max(_characterProfile.bodyWidth, _characterProfile.bodyHeight) * 1.2;
  b2Vec2 vertices[] = {
    {-sideLength / scaleFactor,  sideLength / scaleFactor},
    { sideLength / scaleFactor,  sideLength / scaleFactor},
    {-sideLength / scaleFactor, -sideLength / scaleFactor},
    { sideLength / scaleFactor, -sideLength / scaleFactor}
  };

  // Besides the original fixtures created in Character::defineBody(),
  // create one extra fixture which can collide with player's feetFixture,
  // but make it a sensor. This is the interactable area of this Npc.
  B2BodyBuilder bodyBuilder{_body};
  bodyBuilder.newPolygonFixture(vertices, 4, kPpm)
    .categoryBits(kInteractable)
    .maskBits(kFeet)
    .setSensor(true)
    .setUserData(static_cast<Interactable*>(this))
    .buildFixture();
}

void Npc::import(const string& jsonFilePath) {
  Character::import(jsonFilePath);
  _npcProfile = Npc::Profile{jsonFilePath};
}

void Npc::onSetToKill() {
  Character::onSetToKill();

  if (!_npcProfile.isRespawnable) {
    auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
    gmMgr->setNpcAllowedToSpawn(_characterProfile.jsonFilePath, false);
  }

  dropItems();
}

void Npc::onKilled() {
  Character::onKilled();

  _floatingHealthBar->setVisible(false);
}

void Npc::onMapChanged() {
  _npcController.clearMoveDest();

  if (_isKilled && _party) {
    _party->dismiss(this, /*addToMap=*/false);
  }
}

bool Npc::inflictDamage(Character* target, int damage) {
  if (!Character::inflictDamage(target, damage)) {
    VGLOG(LOG_ERR, "Failed to inflict damage to target: [%p].", target);
    return false;
  }

  // If this Npc is in the player's party, and this Npc kills an enemy,
  // then update player's quests with KillTargetObjective.
  if (_disposition == Npc::Disposition::ALLY && target->isSetToKill() && _party) {
    auto player = dynamic_cast<Player*>(_party->getLeader());
    if (player) {
      player->updateKillTargetObjectives(target);
    }
  }

  return true;
}

bool Npc::receiveDamage(Character* source, int damage) {
  constexpr float kAllyNumSecCantMove = 5.0f;
  if (_disposition == Npc::Disposition::ALLY && !Character::receiveDamage(source, damage, kAllyNumSecCantMove)) {
    VGLOG(LOG_ERR, "Failed to receive damage from source: [%p].", source);
    return false;
  } else if (!Character::receiveDamage(source, damage)) {
    VGLOG(LOG_ERR, "Failed to receive damage from source: [%p].", source);
    return false;
  }

  _isAlerted = true;

  if (!source) {
    _isInvincible = true;
    CallbackManager::the().runAfter([this](const CallbackManager::CallbackId){
      _isInvincible = false;
    }, 1.0f);
  }

  if (_isSetToKill && source) {
    source->addExp(_characterProfile.exp);
  }

  _floatingHealthBar->setVisible(true);
  _floatingHealthBar->update(_characterProfile.health, _characterProfile.fullHealth);

  return true;
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
  // We'll use a callback to drop items since creating fixtures during collision callback
  // will cause the game to crash. Ref: https://github.com/libgdx/libgdx/issues/2730
  CallbackManager::the().runAfter([this](const CallbackManager::CallbackId) {
    auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();

    for (const auto& i : _npcProfile.droppedItems) {
      const string& itemJson = i.first;
      const float dropChance = i.second.chance;

      const float randChance = rand_util::randInt(0, 100);
      if (randChance <= dropChance) {
        int amount = rand_util::randInt(i.second.minAmount, i.second.maxAmount);
        gmMgr->getGameMap()->createItem(itemJson, _killedPos.x * kPpm, _killedPos.y * kPpm, amount);
      }
    }
  }, 0.1f);
}

void Npc::updateDialogueTreeIfNeeded() {
  auto dialogueMgr = SceneManager::the().getCurrentScene<GameScene>()->getDialogueManager();
  string latestDialogueTreeJsonFilePath = dialogueMgr->getLatestNpcDialogueTree(_characterProfile.jsonFilePath);
  if (latestDialogueTreeJsonFilePath.empty()) {
    return;
  }

  VGLOG(LOG_INFO, "Loading dialogue tree from file: [%s], character: [%s]",
                  latestDialogueTreeJsonFilePath.c_str(), _characterProfile.jsonFilePath.c_str());
  _dialogueTree = DialogueTree{latestDialogueTreeJsonFilePath, this};
}

void Npc::onDialogueBegin() {
  auto controlHints = SceneManager::the().getCurrentScene<GameScene>()->getControlHints();
  controlHints->setVisible(false);
}

void Npc::onDialogueEnd() {
  auto controlHints = SceneManager::the().getCurrentScene<GameScene>()->getControlHints();
  controlHints->setVisible(true);
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

bool Npc::isPlayerLeaderOfParty() const {
  return _party ? dynamic_cast<Player*>(_party->getLeader()) != nullptr : false;
}

bool Npc::isWaitingForPlayer() const {
  if (!_party) {
    return false;
  }

  return isPlayerLeaderOfParty() &&
    _party->getWaitingMemberLocationInfo(_characterProfile.jsonFilePath).has_value();
}

bool Npc::isWaitingForPartyLeader() const {
  if (!_party) {
    return false;
  }

  return _party->getWaitingMemberLocationInfo(_characterProfile.jsonFilePath).has_value();
}

void Npc::setDisposition(Npc::Disposition disposition) {
  _disposition = disposition;

  switch (disposition) {
    case Npc::Disposition::ALLY:
      DynamicActor::setCategoryBits(_fixtures[FixtureType::BODY], kAllyBodyCategoryBits);
      DynamicActor::setMaskBits(_fixtures[FixtureType::BODY], kAllyBodyMaskBits);
      DynamicActor::setMaskBits(_fixtures[FixtureType::FEET], kAllyFeetMaskBits);
      DynamicActor::setMaskBits(_fixtures[FixtureType::WEAPON], kAllyWeaponMaskBits);
      break;
    case Npc::Disposition::ENEMY:
      DynamicActor::setCategoryBits(_fixtures[FixtureType::BODY], kEnemyBodyCategoryBits);
      DynamicActor::setMaskBits(_fixtures[FixtureType::BODY], kEnemyBodyMaskBits);
      DynamicActor::setMaskBits(_fixtures[FixtureType::FEET], kEnemyFeetMaskBits);
      DynamicActor::setMaskBits(_fixtures[FixtureType::WEAPON], kEnemyWeaponMaskBits);
      break;
    default:
      VGLOG(LOG_ERR, "Invalid disposition for user: %s", _characterProfile.name.c_str());
      break;
  }
}

Npc::Profile::Profile(const string& jsonFilePath) {
  rapidjson::Document json = json_util::loadFromFile(jsonFilePath);

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
  isTradable = json["isTradable"].GetBool();
  shouldSandbox = json["shouldSandbox"].GetBool();
}

}  // namespace vigilante
