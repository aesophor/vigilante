#include "Player.h"

#include "util/CategoryBits.h"
#include "util/Constants.h"

using cocos2d::Vec2;
using cocos2d::EventListenerKeyboard;
using cocos2d::EventKeyboard;
using cocos2d::Event;
using cocos2d::Director;

namespace vigilante {

Player::Player(float x, float y) {
  defineBody(x, y);
}

Player::~Player() {}


void Player::defineBody(float x, float y) {
  b2World* world = GameMapManager::getInstance()->getWorld();
  float scaleFactor = Director::getInstance()->getContentScaleFactor();

  b2BodyDef bdef;
  bdef.type = b2BodyType::b2_dynamicBody;
  bdef.position.Set(x, y);
  _body = world->CreateBody(&bdef);

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
  
  b2Fixture* bodyFixture = _body->CreateFixture(&fdef);
  bodyFixture->SetUserData(this);
}


b2Body* Player::getBody() const {
  return _body;
}

} // namespace vigilante
