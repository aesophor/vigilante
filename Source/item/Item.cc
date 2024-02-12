// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Item.h"

#include "Assets.h"
#include "Constants.h"
#include "item/Equipment.h"
#include "item/Consumable.h"
#include "item/MiscItem.h"
#include "item/Key.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "util/B2BodyBuilder.h"
#include "util/JsonUtil.h"
#include "util/Logger.h"

using namespace std;
using namespace vigilante::assets;
using namespace vigilante::category_bits;
USING_NS_AX;

namespace vigilante {

namespace {

constexpr int kItemNumAnimations = 0;
constexpr int kItemNumFixtures = 2;

constexpr auto kItemCategoryBits = kItem;
constexpr auto kItemMaskBits = kGround | kPlatform | kWall;

}  // namespace

unique_ptr<Item> Item::create(const string& jsonFilePath) {
  if (jsonFilePath.find("equipment") != jsonFilePath.npos) {
    return std::make_unique<Equipment>(jsonFilePath);
  } else if (jsonFilePath.find("consumable") != jsonFilePath.npos) {
    return std::make_unique<Consumable>(jsonFilePath);
  } else if (jsonFilePath.find("key") != jsonFilePath.npos) {
    return std::make_unique<Key>(jsonFilePath);
  } else if (jsonFilePath.find("misc") != jsonFilePath.npos) {
    return std::make_unique<MiscItem>(jsonFilePath);
  }

  VGLOG(LOG_ERR, "Unable to determine item type.");
  return nullptr;
}

Item::Item(const string& jsonFilePath)
    : DynamicActor{kItemNumAnimations, kItemNumFixtures},
      _itemProfile{jsonFilePath} {
  _bodySprite = Sprite::create(getIconPath());
  _bodySprite->getTexture()->setAliasTexParameters();
}

bool Item::showOnMap(float x, float y) {
  if (_isShownOnMap) {
    return false;
  }

  _isShownOnMap = true;

  defineBody(b2BodyType::b2_dynamicBody, x, y,
             kItemCategoryBits,
             kItemMaskBits);

  _bodySprite = Sprite::create(getIconPath());
  _bodySprite->getTexture()->setAliasTexParameters();

  _node->addChild(_bodySprite, graphical_layers::kItem);

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  gmMgr->getLayer()->addChild(_node, graphical_layers::kItem);

  return true;
}

void Item::import(const string& jsonFilePath) {
  _itemProfile = Item::Profile{jsonFilePath};
}

void Item::defineBody(b2BodyType bodyType,
                      float x,
                      float y,
                      short categoryBits,
                      short maskBits) {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  B2BodyBuilder bodyBuilder(gmMgr->getWorld());

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
    .density(kDensity)
    .setUserData(this)
    .buildFixture();
}

string Item::getIconPath() const {
  return fs::path{_itemProfile.textureResDir} / kIconPng;
}

bool Item::isGold() const {
  return _itemProfile.jsonFilePath == assets::kGoldCoin;
}

Item::Profile::Profile(const string& jsonFilePath) : jsonFilePath(jsonFilePath) {
  rapidjson::Document json = json_util::loadFromFile(jsonFilePath);

  itemType = static_cast<Item::Type>(json["itemType"].GetInt());
  textureResDir = json["textureResDir"].GetString();
  name = json["name"].GetString();
  desc = json["desc"].GetString();
}

}  // namespace vigilante
