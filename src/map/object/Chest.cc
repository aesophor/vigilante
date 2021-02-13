// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Chest.h"

#include <cocos2d.h>
#include "Constants.h"
#include "map/GameMapManager.h"
#include "ui/control_hints/ControlHints.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/JsonUtil.h"

using std::string;
using std::vector;
using cocos2d::Sprite;
using cocos2d::EventKeyboard;
using vigilante::kPpm;
using vigilante::category_bits::kInteractable;
using vigilante::category_bits::kGround;
using vigilante::category_bits::kPlatform;
using vigilante::category_bits::kWall;
using vigilante::category_bits::kFeet;

namespace vigilante {

const int Chest::_kNumAnimations = 0;
const int Chest::_kNumFixtures = 2;

Chest::Chest()
    : DynamicActor(_kNumAnimations, _kNumFixtures),
      _hintBubbleFxSprite(), 
      _itemJsons(),
      _isOpened() {}

Chest::Chest(const string& itemJsons) : Chest() {
  _itemJsons = json_util::splitString(itemJsons);
}


void Chest::showOnMap(float x, float y) {
  if (_isShownOnMap) {
    return;
  }
  _isShownOnMap = true;

  short categoryBits = kInteractable;
  short maskBits = kGround | kPlatform | kWall;
  defineBody(b2BodyType::b2_dynamicBody, categoryBits, maskBits, x, y);

  _bodySprite = Sprite::create("Texture/interactable_object/chest/chest_close.png");
  _bodySprite->getTexture()->setAliasTexParameters();
  GameMapManager::getInstance()->getLayer()->addChild(_bodySprite, graphical_layers::kChest);
}

void Chest::defineBody(b2BodyType bodyType, short categoryBits, short maskBits, float x, float y) {
  b2BodyBuilder bodyBuilder(GameMapManager::getInstance()->getWorld());

  _body = bodyBuilder.type(bodyType)
    .position(x, y, kPpm)
    .buildBody();

  bodyBuilder.newRectangleFixture(16 / 2, 16 / 2, kPpm)
    .categoryBits(categoryBits)
    .maskBits(maskBits)
    .setUserData(this)
    .buildFixture();

  bodyBuilder.newRectangleFixture(16 / 2, 16 / 2, kPpm)
    .categoryBits(kInteractable)
    .maskBits(kFeet)
    .setSensor(true)
    .setUserData(static_cast<Interactable*>(this))
    .buildFixture();
}


void Chest::onInteract(Character*) {
  if (_isOpened) {
    return;
  }
  _isOpened = true;

  _bodySprite->setTexture("Texture/interactable_object/chest/chest_open.png");
  _bodySprite->getTexture()->setAliasTexParameters();

  for (const auto& item : _itemJsons) {
    float x = _body->GetPosition().x;
    float y = _body->GetPosition().y;
    GameMapManager::getInstance()->getGameMap()->createItem(item, x * kPpm, y * kPpm);
  }
  _itemJsons.clear();
}

bool Chest::willInteractOnContact() const {
  return false;
}

void Chest::showHintUI() {
  createHintBubbleFx();
  ControlHints::getInstance()->show(EventKeyboard::KeyCode::KEY_CAPITAL_E, "Open");
}

void Chest::hideHintUI() {
  removeHintBubbleFx();
  ControlHints::getInstance()->hide(EventKeyboard::KeyCode::KEY_CAPITAL_E);
}

void Chest::createHintBubbleFx() {
  if (_hintBubbleFxSprite) {
    removeHintBubbleFx();
  }

  if (_isOpened) {
    return;
  }

  const b2Vec2& bodyPos = _body->GetPosition();
  float x = bodyPos.x * kPpm;
  float y = bodyPos.y * kPpm + HINT_BUBBLE_FX_SPRITE_OFFSET_Y;

  _hintBubbleFxSprite
    = GameMapManager::getInstance()->getFxManager()->createFx(
        "Texture/fx/hint_bubble", "dialogue_available", x, y, -1, 45.0f);
}

void Chest::removeHintBubbleFx() {
  if (!_hintBubbleFxSprite) {
    return;
  }

  _hintBubbleFxSprite->stopAllActions();
  _hintBubbleFxSprite->removeFromParent();
  _hintBubbleFxSprite = nullptr;
}

}  // namespace vigilante
