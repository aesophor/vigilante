#include "Item.h"

#include "json/document.h"

#include "map/GameMapManager.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/CategoryBits.h"
#include "util/Constants.h"
#include "util/JsonUtil.h"

using std::string;
using cocos2d::Sprite;
using vigilante::category_bits::kItem;
using vigilante::category_bits::kFeet;
using vigilante::category_bits::kWall;
using vigilante::category_bits::kGround;
using vigilante::category_bits::kPlatform;
using rapidjson::Document;

namespace vigilante {

const float Item::_kIconWidth = 16.0f;
const float Item::_kIconHeight = 16.0f;

Item::Item(const string& jsonFileName, float x, float y)
    : _itemProfile(jsonFileName),
      _isBodyExisted(),
      _b2body(),
      _sprite(Sprite::create(getIconPath())) {
  // Define b2body and fixture.
  short categoryBits = kItem;
  short maskBits = kGround | kPlatform | kWall;
  defineBody(b2BodyType::b2_dynamicBody, categoryBits, maskBits, x, y);  

  // Disable sprite antialiasing
  _sprite->getTexture()->setAliasTexParameters();
}

Item::~Item() {
  _b2body->GetWorld()->DestroyBody(_b2body);
}


void Item::update(float delta) {
  // Sync the sprite with its b2body.
  b2Vec2 b2bodyPos = _b2body->GetPosition();
  _sprite->setPosition(b2bodyPos.x * kPpm, b2bodyPos.y * kPpm);
}

void Item::defineBody(b2BodyType bodyType, short categoryBits, short maskBits, float x, float y) {
  b2BodyBuilder bodyBuilder(GameMapManager::getInstance()->getWorld());

  _b2body = bodyBuilder.type(bodyType)
    .position(x, y, kPpm)
    .buildBody();

  bodyBuilder.newRectangleFixture(_kIconWidth / 2, _kIconHeight / 2, kPpm)
    .categoryBits(categoryBits)
    .maskBits(maskBits | kFeet) // Enable collision detection with feet fixtures
    .setSensor(true)
    .setUserData(this)
    .buildFixture();

  bodyBuilder.newRectangleFixture(_kIconWidth / 2, _kIconHeight / 2, kPpm)
    .categoryBits(categoryBits)
    .maskBits(maskBits)
    .setUserData(this)
    .buildFixture();

  _isBodyExisted = true;
}

void Item::import(const string& jsonFileName) {
  _itemProfile = Item::Profile(jsonFileName);
}


Item::Profile& Item::getItemProfile() {
  return _itemProfile;
}

std::string Item::getIconPath() const {
  return _itemProfile.textureResPath + "/icon.png";
}


b2Body* Item::getB2Body() const {
  return _b2body;
}

Sprite* Item::getSprite() const {
  return _sprite;
}


Item::Profile::Profile(const string& jsonFileName) {
  Document json = json_util::parseJson(jsonFileName);

  itemType = static_cast<Item::Type>(json["itemType"].GetInt());
  textureResPath = json["textureResPath"].GetString();
  name = json["name"].GetString();
  desc = json["desc"].GetString();
}

} // namespace vigilante
