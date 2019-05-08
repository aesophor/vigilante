#include "Player.h"

#include "map/GameMapManager.h"
#include "util/CategoryBits.h"
#include "util/Constants.h"

using std::string;
using cocos2d::Vector;
using cocos2d::Director;
using cocos2d::Repeat;
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
      _spritesheet(),
      _sprite(),
      _stateTimer(0),
      _isFacingRight(true),
      _isJumping(),
      _isKilled(),
      _isSetToKill() {
  defineBody(x, y);
  defineTexture(x, y);
}

Player::~Player() {
  if (_spritesheet) {
    _spritesheet->release();
  }
}


void Player::update(float delta) {
  //cocos2d::log("[Player::update] x=%f y=%f", _b2body->GetPosition().x, _b2body->GetPosition().y);
  _sprite->setPosition(_b2body->GetPosition().x * kPPM, _b2body->GetPosition().y * kPPM + 5);

  if (!_isFacingRight && !_sprite->isFlippedX()) {
    _sprite->setFlippedX(true);
    runAnimation(State::JUMPING, false);
  } else if (_isFacingRight && _sprite->isFlippedX()) {
    _sprite->setFlippedX(false);
    runAnimation(State::RUNNING);
  }
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
  vertices[0] = {-5 / scaleFactor / kPPM,  20 / scaleFactor / kPPM};
  vertices[1] = { 5 / scaleFactor / kPPM,  20 / scaleFactor / kPPM};
  vertices[2] = {-5 / scaleFactor / kPPM, -14 / scaleFactor / kPPM};
  vertices[3] = { 5 / scaleFactor / kPPM, -14 / scaleFactor / kPPM};
  bodyShape.Set(vertices, 4);

  b2FixtureDef fdef;
  fdef.shape = &bodyShape;
  fdef.filter.categoryBits = kPlayer;
  fdef.filter.maskBits = kGround;
  
  b2Fixture* bodyFixture = _b2body->CreateFixture(&fdef);
  bodyFixture->SetUserData(this);
}

void Player::defineTexture(float x, float y) {
  cocos2d::log("[Player] loading textures");
  SpriteFrameCache* frameCache = SpriteFrameCache::getInstance();

  string location = "Texture/Character/Player/sprites/";
  frameCache->addSpriteFramesWithFile(location + "player.plist");

  _spritesheet = SpriteBatchNode::create(location + "player.png");

  loadAnimation(State::IDLE, "p_idle_sheathed", 6);
  loadAnimation(State::RUNNING, "p_running_sheathed", 8);
  loadAnimation(State::JUMPING, "p_jumping_sheathed", 5);

  // Select a frame as default look of character's sprite.
  _sprite = Sprite::createWithSpriteFrameName("p_running_sheathed/p_running_sheathed0.png");
  _sprite->setPosition(x * kPPM, y * kPPM + 7);
  _sprite->setScale(1.3f);

  _spritesheet->addChild(_sprite);
  _spritesheet->getTexture()->setAliasTexParameters(); // disable texture antialiasing

  runAnimation(State::RUNNING);
  GameMapManager::getInstance()->getScene()->addChild(_spritesheet, 30);
}

void Player::loadAnimation(State state, const string& frameName, size_t frameCount) {
  SpriteFrameCache* frameCache = SpriteFrameCache::getInstance();

  Vector<SpriteFrame*> frames;
  for (size_t i = 0; i < frameCount; i++) {
    string name = frameName + "/" + frameName + std::to_string(i) + ".png";
    frames.pushBack(frameCache->getSpriteFrameByName(name));
  }

  Animation* animation = Animation::createWithSpriteFrames(frames, 0.1);
  animation->retain();
  _animations[state] = animation;
}

void Player::runAnimation(State state, bool loop) const {
  _sprite->stopAllActions();
  auto act = Animate::create(_animations[state]);
  _sprite->runAction((loop) ? (Action*) RepeatForever::create(act) : (Action*) Repeat::create(act, 1));
}

/*
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
*/

void Player::moveLeft() {
  _isFacingRight = false;
  if (_b2body->GetLinearVelocity().x >= -kBaseMovingSpeed * 2) {
    _b2body->ApplyLinearImpulse({-kBaseMovingSpeed, 0}, _b2body->GetWorldCenter(), true);
  }
}

void Player::moveRight() {
  _isFacingRight = true;
  if (_b2body->GetLinearVelocity().x <= kBaseMovingSpeed * 2) {
    _b2body->ApplyLinearImpulse({kBaseMovingSpeed, 0}, _b2body->GetWorldCenter(), true);
  }
}

void Player::jump() {

}


b2Body* Player::getB2Body() const {
  return _b2body;
}

SpriteBatchNode* Player::getSpritesheet() const {
  return _spritesheet;
}

} // namespace vigilante
