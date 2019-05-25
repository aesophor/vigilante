#include "MagicalMissile.h"

#include "GameAssetManager.h"
#include "character/Character.h"
#include "map/GameMapManager.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/CategoryBits.h"
#include "util/Constants.h"

using std::string;
using std::function;
using cocos2d::FileUtils;
using cocos2d::Vector;
using cocos2d::Director;
using cocos2d::Action;
using cocos2d::Animate;
using cocos2d::CallFunc;
using cocos2d::Sequence;
using cocos2d::Repeat;
using cocos2d::RepeatForever;
using cocos2d::Sprite;
using cocos2d::SpriteFrame;
using cocos2d::SpriteBatchNode;
using cocos2d::SpriteFrameCache;
using cocos2d::Animation;
using vigilante::category_bits::kProjectile;
using vigilante::category_bits::kPlayer;
using vigilante::category_bits::kEnemy;
using vigilante::category_bits::kItem;

namespace vigilante {

MagicalMissile::MagicalMissile(const string& jsonFileName, Character* user)
    : DynamicActor(AnimationType::SIZE, 1),
      Skill(),
      Projectile(),
      _skillProfile(jsonFileName),
      _user(user),
      _hasActivated(),
      _hasHit(),
      _launchFxSprite() {}


void MagicalMissile::update(float delta) {
  // Sync the sprite with b2body.
  b2Vec2 bodyPos = _body->GetPosition();
  _bodySprite->setPosition(bodyPos.x * kPpm, bodyPos.y * kPpm);
}

void MagicalMissile::setPosition(float x, float y) {
  _body->SetTransform({x, y}, 0);
}

void MagicalMissile::showOnMap(float x, float y) {
  if (!_isShownOnMap) {
    short categoryBits = kProjectile;
    short maskBits = kPlayer | kEnemy | kItem;
    defineBody(b2BodyType::b2_kinematicBody, categoryBits, maskBits, x, y);

    defineTexture(_skillProfile.textureResDir, x, y);
    GameMapManager::getInstance()->getLayer()->addChild(_bodySpritesheet, 40);
    _isShownOnMap = true;
  }
}

void MagicalMissile::removeFromMap() {
  if (_isShownOnMap) {
    _body->GetWorld()->DestroyBody(_body);

    GameMapManager::getInstance()->getLayer()->removeChild(_bodySpritesheet);
    _isShownOnMap = false;
  }
}


void MagicalMissile::import(const string& jsonFileName) {
  _skillProfile = Skill::Profile(jsonFileName);
}

void MagicalMissile::activate() {
  // Make sure this instance is only activated once.
  if (_hasActivated) {
    return;
  }

  float x = _user->getBody()->GetPosition().x;
  float y = _user->getBody()->GetPosition().y;
  showOnMap(x, y);
  GameMapManager::getInstance()->getGameMap()->getDynamicActors().insert(this);

  // Set up kinematicBody's moving speed.
  _flyingSpeed = (_user->isFacingRight()) ? 2.0f : -2.0f;
  _body->SetLinearVelocity({_flyingSpeed, 0});

  if (!_user->isFacingRight()) {
    _launchFxSprite->setFlippedX(true);
    _bodySprite->setFlippedX(true);
  }

  // Play launch fx animation.
  auto animation = Animate::create(_bodyAnimations[AnimationType::LAUNCH_FX]);
  auto callback = CallFunc::create([=]() {
    _bodySpritesheet->removeChild(_launchFxSprite, true);
  });
  b2Vec2 spellUserBodyPos = _user->getBody()->GetPosition();
  x = spellUserBodyPos.x * kPpm;
  y = spellUserBodyPos.y * kPpm;
  float offset = _user->getCharacterProfile().attackRange;
  x += (_user->isFacingRight()) ? offset : -offset;
  _launchFxSprite->setPosition(x, y);
  _launchFxSprite->runAction(Sequence::createWithTwoActions(animation, callback));

  // Play the spell's main animation.
  Action* action = Repeat::create(Animate::create(_bodyAnimations[AnimationType::FLYING]), 1);
  _bodySprite->runAction(action);

  _hasActivated = true;
}

Skill::Profile& MagicalMissile::getSkillProfile() {
  return _skillProfile;
}

int MagicalMissile::getDamage() const {
  return _skillProfile.physicalDamage + _skillProfile.magicalDamage;
}

Character* MagicalMissile::getUser() const {
  return _user;
}


void MagicalMissile::onHit(Character* target) {
  if (_hasHit) {
    return;
  }

  _body->SetLinearVelocity({_body->GetLinearVelocity().x / 2, 0});

  auto animation = Repeat::create(Animate::create(_bodyAnimations[AnimationType::ON_HIT]), 1);
  auto callback = CallFunc::create([=]() {
    removeFromMap();
    GameMapManager::getInstance()->getGameMap()->getDynamicActors().erase(this);
    delete this;
  });
  _bodySprite->runAction(Sequence::createWithTwoActions(animation, callback));

  _user->inflictDamage(target, getDamage());
  _hasHit = true;
}


void MagicalMissile::defineBody(b2BodyType bodyType, short categoryBits, short maskBits, float x, float y) {
  float spellOffset = _user->getCharacterProfile().attackRange / kPpm;
  spellOffset = (_user->isFacingRight()) ? spellOffset : -spellOffset;

  b2World* world = _user->getBody()->GetWorld();
  b2BodyBuilder bodyBuilder(world);

  _body = bodyBuilder.type(bodyType)
    .position(x + spellOffset, y, 1)
    .buildBody();

  float scaleFactor = Director::getInstance()->getContentScaleFactor();
  b2Vec2 vertices[4];
  vertices[0] = {-10.0f / scaleFactor,  0.0f / scaleFactor};
  vertices[1] = {  0.0f / scaleFactor, -2.0f / scaleFactor};
  vertices[2] = { 10.0f / scaleFactor,  0.0f / scaleFactor};
  vertices[3] = {  0.0f / scaleFactor,  2.0f / scaleFactor};

  _fixtures[0] = bodyBuilder.newPolygonFixture(vertices, 4, kPpm)
    .categoryBits(categoryBits)
    .maskBits(maskBits)
    .setUserData(this)
    .buildFixture();
}

void MagicalMissile::defineTexture(const string& textureResDir, float x, float y) {
  _bodySpritesheet = SpriteBatchNode::create(textureResDir + "/spritesheet.png");

  _bodyAnimations[AnimationType::LAUNCH_FX] = createAnimation(textureResDir, "launch", 5.0f / kPpm);
  _bodyAnimations[AnimationType::FLYING] = createAnimation(textureResDir, "flying", 1.0f / kPpm);
  _bodyAnimations[AnimationType::ON_HIT] = createAnimation(textureResDir, "on_hit", 8.0f / kPpm);

  // Select a frame as default look for this sprite.
  string frameNamePrefix = StaticActor::getLastDirName(textureResDir);
  _launchFxSprite = Sprite::createWithSpriteFrameName(frameNamePrefix + "_launch/0.png");
  _launchFxSprite->setPosition(x, y);
  _bodySprite = Sprite::createWithSpriteFrameName(frameNamePrefix + "_flying/0.png");
  _bodySprite->setPosition(x, y);
  
  _bodySpritesheet->addChild(_launchFxSprite);
  _bodySpritesheet->addChild(_bodySprite);
  _bodySpritesheet->getTexture()->setAliasTexParameters();
}

} // namespace vigilante
