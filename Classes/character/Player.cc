#include "Player.h"

#include "util/CategoryBits.h"
#include "util/Constants.h"

using vigilante::kPlayer;
using vigilante::kGround;
using vigilante::kPPM;
using vigilante::Player;

Player::Player(float x, float y) {
  defineBody(x, y);
}

Player::~Player() {}


void Player::defineBody(float x, float y) {
  b2World* world = GameMapManager::getInstance()->getWorld();

  b2BodyDef bdef;
  bdef.type = b2BodyType::b2_dynamicBody;
  bdef.position.Set(x, y);
  b2Body* b2body = world->CreateBody(&bdef);

  // Create body fixture.
  // Fixture position in box2d is relative to b2body's position.
  b2PolygonShape bodyShape;
  b2Vec2 vertices[4];
  vertices[0] = b2Vec2(-5 / kPPM, 20 / kPPM);
  vertices[1] = b2Vec2(5 / kPPM, 20 / kPPM);
  vertices[2] = b2Vec2(-5 / kPPM, -14 / kPPM);
  vertices[3] = b2Vec2(5 / kPPM, -14 / kPPM);
  bodyShape.Set(vertices, 4);

  b2FixtureDef fdef;
  fdef.shape = &bodyShape;
  fdef.filter.categoryBits = kPlayer;
  fdef.filter.maskBits = kGround;
  
  b2Fixture* bodyFixture = b2body->CreateFixture(&fdef);
  bodyFixture->SetUserData(this);
}
