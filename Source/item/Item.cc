// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Item.h"

#include "Assets.h"
#include "Constants.h"
#include "item/Equipment.h"
#include "item/Consumable.h"
#include "item/MiscItem.h"
#include "item/Key.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/JsonUtil.h"
#include "util/Logger.h"

using namespace std;
using namespace vigilante::category_bits;
USING_NS_AX;

namespace vigilante {

namespace {

constexpr int kItemNumAnimations = 0;
constexpr int kItemNumFixtures = 2;

constexpr auto kItemCategoryBits = kItem;
constexpr auto kItemMaskBits = kGround | kPlatform | kWall;

}  // namespace

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
    : DynamicActor{kItemNumAnimations, kItemNumFixtures},
      _itemProfile{jsonFileName} {
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

void Item::import(const string& jsonFileName) {
  _itemProfile = Item::Profile{jsonFileName};
}

void Item::defineBody(b2BodyType bodyType,
                      float x,
                      float y,
                      short categoryBits,
                      short maskBits) {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  b2BodyBuilder bodyBuilder(gmMgr->getWorld());

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
  return _itemProfile.textureResDir + "/icon.png";
}

bool Item::isGold() const {
  return _itemProfile.jsonFileName == assets::kGoldCoin;
}

Item::Profile::Profile(const string& jsonFileName) : jsonFileName(jsonFileName) {
  rapidjson::Document json = json_util::parseJson(jsonFileName);

  itemType = static_cast<Item::Type>(json["itemType"].GetInt());
  textureResDir = json["textureResDir"].GetString();
  name = json["name"].GetString();
  desc = json["desc"].GetString();
}

}  // namespace vigilante
