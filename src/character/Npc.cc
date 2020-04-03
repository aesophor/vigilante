// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Npc.h"

#include <json/document.h>
#include "AssetManager.h"
#include "Constants.h"
#include "item/Item.h"
#include "map/GameMapManager.h"
#include "ui/dialogue/DialogueManager.h"
#include "util/CallbackUtil.h"
#include "util/JsonUtil.h"
#include "util/RandUtil.h"
#include "util/box2d/b2BodyBuilder.h"

using cocos2d::Action;
using cocos2d::Animate;
using cocos2d::Animation;
using cocos2d::Director;
using cocos2d::Repeat;
using cocos2d::RepeatForever;
using cocos2d::Sprite;
using cocos2d::SpriteBatchNode;
using cocos2d::SpriteFrame;
using cocos2d::SpriteFrameCache;
using cocos2d::Vector;
using rapidjson::Document;
using std::string;
using std::vector;
using vigilante::category_bits::kCliffMarker;
using vigilante::category_bits::kEnemy;
using vigilante::category_bits::kFeet;
using vigilante::category_bits::kGround;
using vigilante::category_bits::kInteractableObject;
using vigilante::category_bits::kItem;
using vigilante::category_bits::kMeleeWeapon;
using vigilante::category_bits::kNpc;
using vigilante::category_bits::kPlatform;
using vigilante::category_bits::kPlayer;
using vigilante::category_bits::kPortal;
using vigilante::category_bits::kProjectile;
using vigilante::category_bits::kWall;

namespace vigilante {

Npc::Npc(const string& jsonFileName)
    : Character(jsonFileName),
      Bot(this),
      _npcProfile(jsonFileName),
      _dialogueTree(_npcProfile.dialogueTree) {}

void Npc::update(float delta) {
  Character::update(delta);
  // act(delta);
}

void Npc::showOnMap(float x, float y) {
  if (_isShownOnMap) {
    return;
  }
  _isShownOnMap = true;
  GameMapManager::getInstance()->getGameMap()->getDynamicActors().insert(this);

  // Construct b2Body and b2Fixtures.
  short bodyCategoryBits = kNpc;
  short bodyMaskBits = kFeet | kMeleeWeapon | kCliffMarker | kProjectile;
  short feetMaskBits = kGround | kPlatform | kWall | kItem | kPortal | kInteractableObject;
  short weaponMaskBits = kPlayer | kEnemy | kNpc;
  defineBody(b2BodyType::b2_dynamicBody, bodyCategoryBits, bodyMaskBits, feetMaskBits,
             weaponMaskBits, x, y);

  // Load sprites, spritesheets, and animations, and then add them to GameMapManager layer.
  defineTexture(_characterProfile.textureResDir, x, y);
  GameMapManager* gmMgr = GameMapManager::getInstance();
  gmMgr->getLayer()->addChild(_bodySpritesheet, graphical_layers::kNpcBody);
  for (auto equipment : _equipmentSlots) {
    if (equipment) {
      Equipment::Type type = equipment->getEquipmentProfile().equipmentType;
      gmMgr->getLayer()->addChild(_equipmentSpritesheets[type],
                                  graphical_layers::kEquipment - type);
    }
  }
}

void Npc::defineBody(b2BodyType bodyType, short bodyCategoryBits, short bodyMaskBits,
                     short feetMaskBits, short weaponMaskBits, float x, float y) {
  Character::defineBody(bodyType, bodyCategoryBits, bodyMaskBits, feetMaskBits, weaponMaskBits,
                        x, y);

  // Besides the original fixtures created in Character::defineBody(),
  // create one extra fixture which can collide with player's feetFixture,
  // but make it a sensor.
  b2BodyBuilder bodyBuilder(_body);

  // Create body fixture.
  // Fixture position in box2d is relative to b2body's position.
  float scaleFactor = Director::getInstance()->getContentScaleFactor();
  b2Vec2 vertices[4];
  float sideLength = std::max(_characterProfile.bodyWidth, _characterProfile.bodyHeight) * 1.5;
  vertices[0] = {-sideLength / scaleFactor, sideLength / scaleFactor};
  vertices[1] = {sideLength / scaleFactor, sideLength / scaleFactor};
  vertices[2] = {-sideLength / scaleFactor, -sideLength / scaleFactor};
  vertices[3] = {sideLength / scaleFactor, -sideLength / scaleFactor};

  bodyBuilder.newPolygonFixture(vertices, 4, kPpm)
      .categoryBits(bodyCategoryBits)
      .maskBits(bodyMaskBits | kFeet)
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
}

void Npc::onInteract(Character* user) {
  auto dialogueMgr = DialogueManager::getInstance();
  dialogueMgr->setTargetNpc(this);
  for (const auto& line : _dialogueTree.getCurrentNode()->lines) {
    dialogueMgr->getSubtitles()->addSubtitle(line);
  }
  dialogueMgr->getSubtitles()->beginSubtitles();
}

bool Npc::willInteractOnContact() const {
  return false;
}

Npc::Profile& Npc::getNpcProfile() {
  return _npcProfile;
}

DialogueTree& Npc::getDialogueTree() {
  return _dialogueTree;
}

Npc::Profile::Profile(const string& jsonFileName) {
  Document json = json_util::parseJson(jsonFileName);

  dialogueTree = json["dialogueTree"].GetString();
}

}  // namespace vigilante
