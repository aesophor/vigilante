// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "Item.h"

#include <cmath>

#include "Assets.h"
#include "Constants.h"
#include "item/Equipment.h"
#include "item/Consumable.h"
#include "item/MiscItem.h"
#include "item/Key.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "util/AxUtil.h"
#include "util/B2BodyBuilder.h"
#include "util/JsonUtil.h"
#include "util/Logger.h"

namespace fs = std::filesystem;
using namespace std;
using namespace requiem::assets;
using namespace requiem::category_bits;
USING_NS_AX;

namespace requiem {

namespace {

constexpr int kItemNumAnimations = 0;
constexpr int kItemNumFixtures = 2;

constexpr auto kItemCategoryBits = kItem;
constexpr auto kItemMaskBits = kGround | kPlatform | kWall;

constexpr float kFloatingSpeedFactor = 3.0f;
constexpr float kFloatingDisplacementFactor = 3.0f;

}  // namespace

unique_ptr<Item> Item::create(const fs::path& jsonFilePath) {
  if (jsonFilePath.native().find("equipment") != jsonFilePath.native().npos) {
    return std::make_unique<Equipment>(jsonFilePath);
  } else if (jsonFilePath.native().find("consumable") != jsonFilePath.native().npos) {
    return std::make_unique<Consumable>(jsonFilePath);
  } else if (jsonFilePath.native().find("key") != jsonFilePath.native().npos) {
    return std::make_unique<Key>(jsonFilePath);
  } else if (jsonFilePath.native().find("misc") != jsonFilePath.native().npos) {
    return std::make_unique<MiscItem>(jsonFilePath);
  }

  VGLOG(LOG_ERR, "Unable to determine item type.");
  return nullptr;
}

Item::Item(const fs::path& jsonFilePath)
    : DynamicActor{kItemNumAnimations, kItemNumFixtures},
      _itemProfile{jsonFilePath} {
  _bodySprite = Sprite::create(getIconPath().native());
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

  _bodySprite = Sprite::create(getIconPath().native());
  _bodySprite->getTexture()->setAliasTexParameters();
  _bodySprite->setScale(0.8f);
  _node->addChild(_bodySprite, z_order::kItem);

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  ax_util::addChildWithParentCameraMask(gmMgr->getLayer(), _node, z_order::kItem);

  return true;
}

void Item::update(const float delta) {
  DynamicActor::update(delta);

  handleVerticalFloatingMovement(delta);
}

void Item::import(const fs::path& jsonFilePath) {
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

fs::path Item::getIconPath() const {
  return _itemProfile.textureResDirPath / kIconPng;
}

bool Item::isGold() const {
  return _itemProfile.jsonFilePath == assets::kGoldCoin;
}

void Item::handleVerticalFloatingMovement(const float delta) {
  _accumulatedDelta += delta;

  const float offsetY = std::sin(_accumulatedDelta * kFloatingSpeedFactor) * kFloatingDisplacementFactor;
  _bodySprite->setPositionY(_bodySprite->getPositionY() + offsetY);
}

Item::Profile::Profile(const fs::path& jsonFilePath) : jsonFilePath(jsonFilePath) {
  rapidjson::Document json = json_util::loadFromFile(jsonFilePath);

  textureResDirPath = json["textureResDirPath"].GetString();
  itemType = static_cast<Item::Type>(json["itemType"].GetInt());
  name = json["name"].GetString();
  desc = json["desc"].GetString();
  price = json["price"].GetInt();
}

}  // namespace requiem
