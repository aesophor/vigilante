#include "WorldContactListener.h"

#include "character/Player.h"
#include "util/CategoryBits.h"


namespace vigilante {

void WorldContactListener::BeginContact(b2Contact* contact) {
  b2Fixture* fixtureA = contact->GetFixtureA();
  b2Fixture* fixtureB = contact->GetFixtureB();

  int cDef = fixtureA->GetFilterData().categoryBits | fixtureB->GetFilterData().categoryBits;
  switch (cDef) {
    case category_bits::kPlayer | category_bits::kGround:
      if (fixtureA->GetFilterData().categoryBits == category_bits::kPlayer) {
        static_cast<Player*>(fixtureA->GetUserData())->setIsJumping(false);
      } else {
        static_cast<Player*>(fixtureB->GetUserData())->setIsJumping(false);
      }
      break;
    default:
      break;
  }
}

void WorldContactListener::EndContact(b2Contact* contact) {

}

void WorldContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {

}

void WorldContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {

}

} // namespace vigilante
