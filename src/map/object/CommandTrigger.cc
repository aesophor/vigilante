// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "CommandTrigger.h"

#include "Constants.h"
#include "map/GameMapManager.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/JsonUtil.h"

#define COMMAND_TRIGGER_NUM_ANIMATIONS 0
#define COMMAND_TRIGGER_NUM_FIXTURES 2

#define COMMAND_TRIGGER_CATEGORY_BITS kInteractable
#define COMMAND_TRIGGER_MASK_BITS kGround | kPlatform | kWall

using cocos2d::Sprite;
using vigilante::category_bits::kInteractable;
using vigilante::category_bits::kGround;
using vigilante::category_bits::kPlatform;
using vigilante::category_bits::kWall;
using vigilante::category_bits::kFeet;

namespace vigilante {

CommandTrigger::CommandTrigger()
    : DynamicActor(COMMAND_TRIGGER_NUM_ANIMATIONS, COMMAND_TRIGGER_NUM_FIXTURES) {}


void CommandTrigger::showOnMap(float x, float y) {
  if (_isShownOnMap) {
    return;
  }
  _isShownOnMap = true;

  defineBody(b2BodyType::b2_dynamicBody, x, y,
             COMMAND_TRIGGER_CATEGORY_BITS, COMMAND_TRIGGER_MASK_BITS);

  _bodySprite = Sprite::create();
  _bodySprite->getTexture()->setAliasTexParameters();
  GameMapManager::getInstance()->getLayer()->addChild(_bodySprite);
}

void CommandTrigger::defineBody(b2BodyType bodyType,
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


void CommandTrigger::onInteract(Character*) {

}

bool CommandTrigger::willInteractOnContact() const {
  return true;
}

void CommandTrigger::showHintUI() {

}

void CommandTrigger::hideHintUI() {

}

void CommandTrigger::createHintBubbleFx() {

}

void CommandTrigger::removeHintBubbleFx() {

}


}  // namespace vigilante
