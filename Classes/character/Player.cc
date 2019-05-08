#include "Player.h"

#include "util/CategoryBits.h"
#include "util/Constants.h"

using cocos2d::Vec2;
using cocos2d::EventListenerKeyboard;
using cocos2d::EventKeyboard;
using cocos2d::Event;
using cocos2d::Director;

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
}

Player::~Player() {}


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

} // namespace vigilante
