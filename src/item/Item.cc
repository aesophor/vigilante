// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Item.h"

#include <json/document.h>
#include "AssetManager.h"
#include "Constants.h"
#include "std/make_unique.h"
#include "item/Equipment.h"
#include "item/Consumable.h"
#include "item/MiscItem.h"
#include "item/Key.h"
#include "map/GameMapManager.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/JsonUtil.h"
#include "util/Logger.h"

#define ITEM_NUM_ANIMATIONS 0
#define ITEM_NUM_FIXTURES 2

#define ITEM_CATEGORY_BITS kItem
#define ITEM_MASK_BITS kGround | kPlatform | kWall

using std::string;
using std::unique_ptr;
using cocos2d::Sprite;
using vigilante::category_bits::kItem;
using vigilante::category_bits::kFeet;
using vigilante::category_bits::kWall;
using vigilante::category_bits::kGround;
using vigilante::category_bits::kPlatform;
using rapidjson::Document;

namespace vigilante {

unique_ptr<Item> Item::create(const string& jsonFileName) {
  if (jsonFileName.find("equipment") != jsonFileName.npos) {
    return std::make_unique<Equipment>(jsonFileName);
  } else if (jsonFileName.find("consumable") != jsonFileName.npos) {
    return std::make_unique<Consumable>(jsonFileName);
  } else if (jsonFileName.find("key") != jsonFileName.npos) {
    return std::make_unique<Key>(jsonFileName);
  } else if (jsonFileName.find("misc") != jsonFileName.npos) {
    return std::make_unique<MiscItem>(jsonFileName);
  }

  VGLOG(LOG_ERR, "Unable to determine item type.");
  return nullptr;
}

Item::Item(const string& jsonFileName)
    : DynamicActor(ITEM_NUM_ANIMATIONS, ITEM_NUM_FIXTURES),
      _itemProfile(jsonFileName),
      _amount(1) {
  _bodySprite = Sprite::create(getIconPath());
  _bodySprite->getTexture()->setAliasTexParameters();
}


bool Item::showOnMap(float x, float y) {
  if (_isShownOnMap) {
    return false;
  }

  _isShownOnMap = true;

  defineBody(b2BodyType::b2_dynamicBody,
             x,
             y,
             ITEM_CATEGORY_BITS,
             ITEM_MASK_BITS);  

  _bodySprite = Sprite::create(getIconPath());
  _bodySprite->getTexture()->setAliasTexParameters();
  GameMapManager::getInstance()->getLayer()->addChild(_bodySprite,
                                                      graphical_layers::kItem);
  return true;
}

void Item::import(const string& jsonFileName) {
  _itemProfile = Item::Profile(jsonFileName);
}


void Item::defineBody(b2BodyType bodyType,
                      float x,
                      float y,
                      short categoryBits,
                      short maskBits) {
  b2BodyBuilder bodyBuilder(GameMapManager::getInstance()->getWorld());

  _body = bodyBuilder.type(bodyType)
    .position(x, y, kPpm)
    .buildBody();

  bodyBuilder.newRectangleFixture(kIconSize / 2, kIconSize / 2, kPpm)
    .categoryBits(categoryBits)
    .maskBits(maskBits | kFeet)  // Enable collision detection with feet fixtures
    .setSensor(true)
    .setUserData(this)
    .buildFixture();

  bodyBuilder.newRectangleFixture(kIconSize / 2, kIconSize / 2, kPpm)
    .categoryBits(categoryBits)
    .maskBits(maskBits)
    .setUserData(this)
    .buildFixture();
}


Item::Profile& Item::getItemProfile() {
  return _itemProfile;
}

const string& Item::getName() const {
  return _itemProfile.name;
}

const string& Item::getDesc() const {
  return _itemProfile.desc;
}

string Item::getIconPath() const {
  return _itemProfile.textureResDir + "/icon.png";
}

bool Item::isGold() const {
  return _itemProfile.jsonFileName == asset_manager::kGoldCoin;
}


int Item::getAmount() const {
  return _amount;
}

void Item::setAmount(int amount) {
  _amount = amount;
}


Item::Profile::Profile(const string& jsonFileName) : jsonFileName(jsonFileName) {
  Document json = json_util::parseJson(jsonFileName);

  itemType = static_cast<Item::Type>(json["itemType"].GetInt());
  textureResDir = json["textureResDir"].GetString();
  name = json["name"].GetString();
  desc = json["desc"].GetString();
}

}  // namespace vigilante
