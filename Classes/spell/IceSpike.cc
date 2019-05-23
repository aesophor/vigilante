#include "IceSpike.h"

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

IceSpike::IceSpike(Character* spellUser, float flyingSpeed)
    : DynamicActor(AnimationType::SIZE, 1),
      _spellUser(spellUser),
      _damage(50),
      _flyingSpeed(flyingSpeed),
      _hasActivated(),
      _launchFxSprite() {
  short categoryBits = kProjectile;
  short maskBits = kPlayer | kEnemy | kItem;
  float x = spellUser->getBody()->GetPosition().x;
  float y = spellUser->getBody()->GetPosition().y;
  defineBody(b2BodyType::b2_kinematicBody, categoryBits, maskBits, x, y);
  defineTexture("Texture/spell/ice_spike", x * kPpm, y * kPpm);
}

IceSpike::~IceSpike() {}

void IceSpike::update(float delta) {
  // Sync the sprite with b2body.
  b2Vec2 bodyPos = _body->GetPosition();
  _bodySprite->setPosition(bodyPos.x * kPpm, bodyPos.y * kPpm);
}

void IceSpike::setPosition(float x, float y) {
  _body->SetTransform({x, y}, 0);
}

void IceSpike::activate() {
  if (_hasActivated) {
    return;
  }

  // Set up kinematicBody's moving speed.
  _flyingSpeed = (_spellUser->isFacingRight()) ? _flyingSpeed : -_flyingSpeed;
  _body->SetLinearVelocity({_flyingSpeed, 0});

  if (!_spellUser->isFacingRight()) {
    _launchFxSprite->setFlippedX(true);
    _bodySprite->setFlippedX(true);
  }

  // Play launch fx animation.
  auto animation = Animate::create(_bodyAnimations[AnimationType::LAUNCH_FX]);
  auto callback = CallFunc::create([=]() {
    _bodySpritesheet->removeChild(_launchFxSprite, true);
  });
  b2Vec2 spellUserBodyPos = _spellUser->getBody()->GetPosition();
  float x = spellUserBodyPos.x * kPpm;
  float y = spellUserBodyPos.y * kPpm;
  float offset = _spellUser->getProfile().attackRange;
  x += (_spellUser->isFacingRight()) ? offset : -offset;
  _launchFxSprite->setPosition(x, y);
  _launchFxSprite->runAction(Sequence::createWithTwoActions(animation, callback));

  // Play the spell's main animation.
  Action* action = Repeat::create(Animate::create(_bodyAnimations[AnimationType::FLYING]), 1);
  _bodySprite->runAction(action);

  _hasActivated = true;
}

void IceSpike::onHit() {
  auto animation = Repeat::create(Animate::create(_bodyAnimations[AnimationType::ON_HIT]), 1);
  auto callback = CallFunc::create([=]() {
    GameMapManager::getInstance()->getLayer()->removeChild(_bodySpritesheet);
    GameMapManager::getInstance()->getGameMap()->getInUseSpells().erase(this);
    // FIXME
    //delete this; // maybe this is not a good idea....
    cocos2d::log("destoying body");
    _body->SetLinearVelocity({0, 0});
    _body->GetWorld()->DestroyBody(_body);
  });

  _bodySprite->runAction(Sequence::createWithTwoActions(animation, callback));
}


void IceSpike::defineBody(b2BodyType bodyType, short categoryBits, short maskBits, float x, float y) {
  float spellOffset = _spellUser->getProfile().attackRange / kPpm;
  spellOffset = (_spellUser->isFacingRight()) ? spellOffset : -spellOffset;

  b2World* world = _spellUser->getBody()->GetWorld();
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

void IceSpike::defineTexture(const string& textureResPath, float x, float y) {
  _bodySpritesheet = SpriteBatchNode::create(textureResPath + "/spritesheet.png");

  _bodyAnimations[AnimationType::LAUNCH_FX] = createAnimation(textureResPath, "launch", 5.0f / kPpm);
  _bodyAnimations[AnimationType::FLYING] = createAnimation(textureResPath, "flying", 1.0f / kPpm);
  _bodyAnimations[AnimationType::ON_HIT] = createAnimation(textureResPath, "on_hit", 8.0f / kPpm);

  // Select a frame as default look for this sprite.
  string frameNamePrefix = asset_manager::getFrameNamePrefix(textureResPath);
  _launchFxSprite = Sprite::createWithSpriteFrameName(frameNamePrefix + "_launch/0.png");
  _launchFxSprite->setPosition(x, y);
  _bodySprite = Sprite::createWithSpriteFrameName(frameNamePrefix + "_flying/0.png");
  _bodySprite->setPosition(x, y);
  
  _bodySpritesheet->addChild(_launchFxSprite);
  _bodySpritesheet->addChild(_bodySprite);
  _bodySpritesheet->getTexture()->setAliasTexParameters();
}


Character* IceSpike::getSpellUser() const {
  return _spellUser;
}

int IceSpike::getDamage() const {
  return _damage;
}

} // namespace vigilante
