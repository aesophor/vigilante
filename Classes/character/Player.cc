#include "Player.h"

#include "util/CategoryBits.h"
#include "util/Constants.h"

using std::string;
using cocos2d::Vector;
using cocos2d::Director;
using cocos2d::RepeatForever;
using cocos2d::Animation;
using cocos2d::Animate;
using cocos2d::Action;
using cocos2d::Sprite;
using cocos2d::SpriteFrame;
using cocos2d::SpriteFrameCache;
using cocos2d::SpriteBatchNode;


namespace vigilante {

const float Player::kBaseMovingSpeed = .25f;

Player::Player(float x, float y)
    : _b2body(),
      _bodyFixture(),
      _stateTimer(0),
      _isFacingRight(true),
      _isJumping(),
      _isKilled(),
      _isSetToKill() {
  defineBody(x, y);
  defineAnimations(x, y);
}

Player::~Player() {
  if (_spritesheet) {
    _spritesheet->release();
  }
}


void Player::update(float delta) {
  auto boundingBox = _sprite->getBoundingBox();
  float w = boundingBox.size.width;
  float h = boundingBox.size.height;
  //cocos2d::log("[Player::update] x=%f y=%f", _b2body->GetPosition().x, _b2body->GetPosition().y);
  _sprite->setPosition(_b2body->GetPosition().x * kPPM, _b2body->GetPosition().y * kPPM + 5);
}

void Player::defineBody(float x, float y) {
  b2World* world = GameMapManager::getInstance()->getWorld();
  float scaleFactor = Director::getInstance()->getContentScaleFactor();

  b2BodyDef bdef;
  bdef.type = b2BodyType::b2_dynamicBody;
  bdef.position.Set(x, y);
  _b2body = world->CreateBody(&bdef);

  // Create body fixture.
  // Fixture position in box2d is relative to b2body's position.
  b2PolygonShape bodyShape;
  b2Vec2 vertices[4];
  vertices[0] = b2Vec2(-5 / scaleFactor / kPPM, 20 / scaleFactor / kPPM);
  vertices[1] = b2Vec2(5 / scaleFactor / kPPM, 20 / scaleFactor / kPPM);
  vertices[2] = b2Vec2(-5 / scaleFactor / kPPM, -14 / scaleFactor / kPPM);
  vertices[3] = b2Vec2(5 / scaleFactor / kPPM, -14 / scaleFactor / kPPM);
  bodyShape.Set(vertices, 4);

  b2FixtureDef fdef;
  fdef.shape = &bodyShape;
  fdef.filter.categoryBits = kPlayer;
  fdef.filter.maskBits = kGround;
  
  b2Fixture* bodyFixture = _b2body->CreateFixture(&fdef);
  bodyFixture->SetUserData(this);
}

void Player::defineAnimations(float x, float y) {
  SpriteFrameCache* frameCache = SpriteFrameCache::getInstance();

  string location = "Texture/Character/Player/sprites/p_attacking/";
  frameCache->addSpriteFramesWithFile(location + "p_attacking.plist");

  _spritesheet = SpriteBatchNode::create(location + "p_attacking.png");

  Vector<SpriteFrame*> pAttackingFrames;
  for (int i = 0; i <= 7; i++) {
    SpriteFrame* frame = frameCache->getSpriteFrameByName("p_attacking" + std::to_string(i) + ".png");
    pAttackingFrames.pushBack(frame);
  }

  Animation* animation = Animation::createWithSpriteFrames(pAttackingFrames, 0.1);
  _sprite = Sprite::createWithSpriteFrameName("p_attacking0.png");
  _sprite->setPosition(x * kPPM, y * kPPM + 7);
  _sprite->setScale(1.3f);

  Action* action = RepeatForever::create(Animate::create(animation));
  _sprite->runAction(action);
  _spritesheet->addChild(_sprite);
  _spritesheet->getTexture()->setAliasTexParameters(); // disable texture antialiasing
}

Action* Player::defineFrames(const string& frameName) {
  SpriteFrameCache* frameCache = SpriteFrameCache::getInstance();
  frameCache->addSpriteFramesWithFile(frameName + ".plist");

  _spritesheet = SpriteBatchNode::create(frameName + ".png");

  Vector<SpriteFrame*> frames;
  for (int i = 0; i <= 7; i++) {
    SpriteFrame* frame = frameCache->getSpriteFrameByName("p_attacking" + std::to_string(i) + ".png");
    frames.pushBack(frame);
  }

  Animation* animation = Animation::createWithSpriteFrames(frames, 0.1);
  _sprite = Sprite::createWithSpriteFrameName("p_attacking0.png");
  //_sprite->setPosition(x * kPPM, y * kPPM + 7);
  _sprite->setScale(1.3f);

  Action* action = RepeatForever::create(Animate::create(animation));
  _sprite->runAction(action);
  _spritesheet->addChild(_sprite);
  _spritesheet->getTexture()->setAliasTexParameters(); // disable texture antialiasing
}


void Player::moveLeft() const {
  if (_b2body->GetLinearVelocity().x >= -kBaseMovingSpeed * 2) {
    _b2body->ApplyLinearImpulse({-kBaseMovingSpeed, 0}, _b2body->GetWorldCenter(), true);
  }
}

void Player::moveRight() const {
  if (_b2body->GetLinearVelocity().x <= kBaseMovingSpeed * 2) {
    _b2body->ApplyLinearImpulse({kBaseMovingSpeed, 0}, _b2body->GetWorldCenter(), true);
  }
}


b2Body* Player::getB2Body() const {
  return _b2body;
}

SpriteBatchNode* Player::getSpritesheet() const {
  return _spritesheet;
}

} // namespace vigilante
