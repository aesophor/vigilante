// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Chest.h"

#include <cocos2d.h>
#include "Constants.h"
#include "map/GameMapManager.h"
#include "util/box2d/b2BodyBuilder.h"

using std::string;
using std::vector;
using cocos2d::Sprite;
using vigilante::kPpm;
using vigilante::category_bits::kInteractableObject;
using vigilante::category_bits::kGround;
using vigilante::category_bits::kPlatform;
using vigilante::category_bits::kWall;
using vigilante::category_bits::kFeet;

namespace vigilante {

const int Chest::_kNumAnimations = 0;
const int Chest::_kNumFixtures = 2;

Chest::Chest() : DynamicActor(_kNumAnimations, _kNumFixtures), _isOpened() {}


void Chest::showOnMap(float x, float y) {
  if (_isShownOnMap) {
    return;
  }

  _isShownOnMap = true;
  GameMapManager::getInstance()->getGameMap()->addDynamicActor(this);

  short categoryBits = kInteractableObject;
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
    .maskBits(maskBits | kFeet)
    .setSensor(true)
    .setUserData(this)
    .buildFixture();

  bodyBuilder.newRectangleFixture(16 / 2, 16 / 2, kPpm)
    .categoryBits(categoryBits)
    .maskBits(maskBits)
    .setUserData(this)
    .buildFixture();
}


vector<string>& Chest::getItemJsons() {
  return _itemJsons;
}


void Chest::onInteract(Character* user) {
  if (!_isOpened) {
    _bodySprite->setTexture("Texture/interactable_object/chest/chest_open.png");
    _bodySprite->getTexture()->setAliasTexParameters();
    _isOpened = true;

    for (const auto item : _itemJsons) {
      float x = _body->GetPosition().x;
      float y = _body->GetPosition().y;
      GameMapManager::getInstance()->getGameMap()->spawnItem(item, x * kPpm, y * kPpm);
    }
    _itemJsons.clear();
  }
}

bool Chest::willInteractOnContact() const {
  return false;
}

} // namespace 
