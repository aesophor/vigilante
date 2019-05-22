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

IceSpike::IceSpike(Character* spellUser)
    : _spellUser(spellUser),
      _damage(50),
      _hasActivated(),
      _body(),
      _fixture(),
      _spritesheet(),
      _launchFxSprite(),
      _sprite(),
      _launchFxAnimation(),
      _flyingAnimation(),
      _onHitAnimation() {
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
  _sprite->setPosition(bodyPos.x * kPpm, bodyPos.y * kPpm);
}

void IceSpike::reposition(float x, float y) {
  _body->SetTransform({x, y}, 0);
}

void IceSpike::activate(float speed) {
  if (_hasActivated) {
    return;
  }

  // Set up kinematicBody's moving speed.
  speed = (_spellUser->isFacingRight()) ? speed : -speed;
  _body->SetLinearVelocity({speed, 0});

  if (!_spellUser->isFacingRight()) {
    _launchFxSprite->setFlippedX(true);
    _sprite->setFlippedX(true);
  }

  // Play launch fx animation.
  auto animation = Animate::create(_launchFxAnimation);
  auto callback = CallFunc::create([=]() {
    _spritesheet->removeChild(_launchFxSprite, true);
  });
  b2Vec2 spellUserBodyPos = _spellUser->getBody()->GetPosition();
  float x = spellUserBodyPos.x * kPpm;
  float y = spellUserBodyPos.y * kPpm;
  float offset = _spellUser->getProfile().attackRange;
  x += (_spellUser->isFacingRight()) ? offset : -offset;
  _launchFxSprite->setPosition(x, y);
  _launchFxSprite->runAction(Sequence::createWithTwoActions(animation, callback));

  // Play the spell's main animation.
  Action* action = Repeat::create(Animate::create(_flyingAnimation), 1);
  _sprite->runAction(action);

  _hasActivated = true;
}

void IceSpike::onHit() {
  auto animation = Repeat::create(Animate::create(_onHitAnimation), 1);
  auto callback = CallFunc::create([=]() {
    GameMapManager::getInstance()->getLayer()->removeChild(_spritesheet);
    GameMapManager::getInstance()->getGameMap()->getInUseSpells().erase(this);
    // FIXME
    //delete this; // maybe this is not a good idea....
    cocos2d::log("destoying body");
    _body->SetLinearVelocity({0, 0});
    _body->GetWorld()->DestroyBody(_body);
  });

  _sprite->runAction(Sequence::createWithTwoActions(animation, callback));
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

  _fixture = bodyBuilder.newPolygonFixture(vertices, 4, kPpm)
    .categoryBits(categoryBits)
    .maskBits(maskBits)
    .setUserData(this)
    .buildFixture();
}

void IceSpike::defineTexture(const string& textureResPath, float x, float y) {
  _spritesheet = SpriteBatchNode::create(textureResPath + "/spritesheet.png");

  _launchFxAnimation = createAnimation(textureResPath, "launch", 5.0f / kPpm);
  _flyingAnimation = createAnimation(textureResPath, "flying", 1.0f / kPpm);
  _onHitAnimation = createAnimation(textureResPath, "onhit", 8.0f / kPpm);

  // Select a frame as default look for this sprite.
  string frameNamePrefix = asset_manager::getFrameNamePrefix(textureResPath);
  _launchFxSprite = Sprite::createWithSpriteFrameName(frameNamePrefix + "_launch/0.png");
  _launchFxSprite->setPosition(x, y);
  _sprite = Sprite::createWithSpriteFrameName(frameNamePrefix + "_flying/0.png");
  _sprite->setPosition(x, y);
  
  _spritesheet->addChild(_launchFxSprite);
  _spritesheet->addChild(_sprite);
  _spritesheet->getTexture()->setAliasTexParameters();
}

Animation* IceSpike::createAnimation(const string& textureResPath, string frameName, float delay) const {
  SpriteFrameCache* frameCache = SpriteFrameCache::getInstance();

  // Texture/character/player/player_attacking/0.png
  // |______________________| |____| |_______| |___|
  //      textureResPath         |   frameName
  //                      frameNamePrefix

  // frameNamePrefix is used to prevent against frame name collisions.
  string frameNamePrefix = asset_manager::getFrameNamePrefix(textureResPath);

  // Count how many frames (.png) are there in the corresponding directory.
  // Method: we will use FileUtils to test whether a file exists starting from 0.png, 1.png, ..., n.png
  size_t frameCount = 0;
  while (FileUtils::getInstance()->isFileExist(textureResPath + "/" + frameNamePrefix + "_" + frameName + "/" + std::to_string(frameCount) + ".png")) {
    frameCount++;
  }

  Vector<SpriteFrame*> frames;
  for (size_t i = 0; i < frameCount; i++) {
    const string& name = frameNamePrefix + "_" + frameName + "/" + std::to_string(i) + ".png";
    frames.pushBack(frameCache->getSpriteFrameByName(name));
  }
  Animation* animation = Animation::createWithSpriteFrames(frames, delay);
  animation->retain();
  return animation;
}


Character* IceSpike::getSpellUser() const {
  return _spellUser;
}

const int IceSpike::getDamage() const {
  return _damage;
}


b2Body* IceSpike::getBody() const {
  return _body;
}

b2Fixture* IceSpike::getFixture() const {
  return _fixture;
}


Sprite* IceSpike::getSprite() const {
  return _sprite;
}

SpriteBatchNode* IceSpike::getSpritesheet() const {
  return _spritesheet;
}

} // namespace vigilante
