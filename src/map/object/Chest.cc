// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Chest.h"

#include "AudioManager.h"
#include "Assets.h"
#include "Constants.h"
#include "map/FxManager.h"
#include "map/GameMapManager.h"
#include "ui/control_hints/ControlHints.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/JsonUtil.h"
#include "util/StringUtil.h"

#define CHEST_NUM_ANIMATIONS 0
#define CHEST_NUM_FIXTURES 2

#define ITEM_CATEGORY_BITS kInteractable
#define ITEM_MASK_BITS kGround | kPlatform | kWall

using namespace std;
using namespace vigilante::assets;
using namespace vigilante::category_bits;
USING_NS_CC;

namespace vigilante {

Chest::Chest()
    : DynamicActor(CHEST_NUM_ANIMATIONS, CHEST_NUM_FIXTURES),
      _hintBubbleFxSprite(),
      _itemJsons(),
      _isOpened() {}

Chest::Chest(const string& itemJsons) : Chest() {
  _itemJsons = string_util::split(itemJsons);
}

bool Chest::showOnMap(float x, float y) {
  if (_isShownOnMap) {
    return false;
  }

  _isShownOnMap = true;

  defineBody(b2BodyType::b2_dynamicBody,
             x,
             y,
             ITEM_CATEGORY_BITS,
             ITEM_MASK_BITS);

  _bodySprite = Sprite::create("Texture/interactable_object/chest/chest_close.png");
  _bodySprite->getTexture()->setAliasTexParameters();
  GameMapManager::getInstance()->getLayer()->addChild(_bodySprite,
                                                      graphical_layers::kChest);
  return true;
}

void Chest::defineBody(b2BodyType bodyType,
                       float x,
                       float y,
                       short categoryBits,
                       short maskBits) {
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
  AudioManager::getInstance()->playSfx(kSfxChestOpened);

  for (const auto& item : _itemJsons) {
    float x = _body->GetPosition().x;
    float y = _body->GetPosition().y;
    GameMapManager::getInstance()->getGameMap()->createItem(item, x * kPpm, y * kPpm);
  }
  _itemJsons.clear();
}

void Chest::showHintUI() {
  if (_isOpened) {
    return;
  }

  createHintBubbleFx();
  ControlHints::getInstance()->insert({EventKeyboard::KeyCode::KEY_CAPITAL_E}, "Open");
}

void Chest::hideHintUI() {
  removeHintBubbleFx();
  ControlHints::getInstance()->remove({EventKeyboard::KeyCode::KEY_CAPITAL_E});
}

void Chest::createHintBubbleFx() {
  if (_hintBubbleFxSprite) {
    removeHintBubbleFx();
  }

  if (_isOpened) {
    return;
  }

  _hintBubbleFxSprite
    = FxManager::getInstance()->createHintBubbleFx(_body, "dialogue_available");
}

void Chest::removeHintBubbleFx() {
  if (!_hintBubbleFxSprite) {
    return;
  }

  FxManager::getInstance()->removeFx(_hintBubbleFxSprite);
  _hintBubbleFxSprite = nullptr;
}

}  // namespace vigilante
