#include "WorldContactListener.h"

#include "cocos2d.h"

#include "character/Player.h"
#include "util/CategoryBits.h"


namespace vigilante {

void WorldContactListener::BeginContact(b2Contact* contact) {
  b2Fixture* fixtureA = contact->GetFixtureA();
  b2Fixture* fixtureB = contact->GetFixtureB();

  int cDef = fixtureA->GetFilterData().categoryBits | fixtureB->GetFilterData().categoryBits;
  switch (cDef) {
    // When a character lands on the ground, make following changes.
    case category_bits::kFeet | category_bits::kGround: {
      b2Fixture* feetFixture = GetTargetFixture(category_bits::kFeet, fixtureA, fixtureB);
      if (feetFixture) {
        static_cast<Player*>(feetFixture->GetUserData())->setIsJumping(false);
      }
      break;
    }
    // When a character lands on a platform, make following changes.
    case category_bits::kFeet | category_bits::kPlatform: {
      b2Fixture* feetFixture = GetTargetFixture(category_bits::kFeet, fixtureA, fixtureB);
      if (feetFixture) {
        Player* player = static_cast<Player*>(feetFixture->GetUserData());
        player->setIsJumping(false);
        player->setIsOnPlatform(true);
      }
      break;
    }
    default:
      break;
  }
}

void WorldContactListener::EndContact(b2Contact* contact) {

}

void WorldContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {
  b2Fixture* fixtureA = contact->GetFixtureA();
  b2Fixture* fixtureB = contact->GetFixtureB();

  int cDef = fixtureA->GetFilterData().categoryBits | fixtureB->GetFilterData().categoryBits;
  switch (cDef) {
    // Allow player to pass through platforms and collide on the way down.
    case category_bits::kFeet | category_bits::kPlatform: {
      b2Fixture* playerBody = GetTargetFixture(category_bits::kFeet, fixtureA, fixtureB);
      b2Fixture* platform = GetTargetFixture(category_bits::kPlatform, fixtureA, fixtureB);

      float playerY = playerBody->GetBody()->GetPosition().y;
      float platformY = platform->GetBody()->GetPosition().y;

      // Enable contact if the player is about to land on the platform.
      // .15f is a value that works fine in my world.
      contact->SetEnabled((playerY > platformY + .10f));
      break;
    }
    default:
      break;
  }
}

void WorldContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {

}


b2Fixture* WorldContactListener::GetTargetFixture(short targetCategoryBits, b2Fixture* f1, b2Fixture* f2) const {
  b2Fixture* targetFixture = nullptr;

  if (f1->GetFilterData().categoryBits == targetCategoryBits) {
    targetFixture = f1;
  } else if (f2->GetFilterData().categoryBits == targetCategoryBits) {
    targetFixture = f2;
  }
  return targetFixture;
}

} // namespace vigilante
