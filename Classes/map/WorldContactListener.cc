#include "WorldContactListener.h"

#include "cocos2d.h"

#include "character/Player.h"
#include "character/Enemy.h"
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
        static_cast<Character*>(feetFixture->GetUserData())->setIsJumping(false);
      }
      break;
    }
    // When a character lands on a platform, make following changes.
    case category_bits::kFeet | category_bits::kPlatform: {
      b2Fixture* feetFixture = GetTargetFixture(category_bits::kFeet, fixtureA, fixtureB);
      if (feetFixture) {
        Character* c = static_cast<Character*>(feetFixture->GetUserData());
        c->setIsJumping(false);
        c->setIsOnPlatform(true);
      }
      break;
    }
    // When a player bumps into an enemy, the enemy will inflict damage to the player and knock it back.
    case category_bits::kPlayer | category_bits::kEnemy: {
      b2Fixture* playerFixture = GetTargetFixture(category_bits::kPlayer, fixtureA, fixtureB);
      b2Fixture* enemyFixture = GetTargetFixture(category_bits::kEnemy, fixtureA, fixtureB);

      if (playerFixture && enemyFixture) {
        Player* player = static_cast<Player*>(playerFixture->GetUserData());
        Enemy* enemy = static_cast<Enemy*>(enemyFixture->GetUserData());

        enemy->inflictDamage(player, 25);
        float knockBackForceX = (player->isFacingRight()) ? -.25f : .25f; // temporary
        float knockBackForceY = 1.0f; // temporary
        enemy->knockBack(player, knockBackForceX, knockBackForceY);
      }
      break;
    }
    case category_bits::kEnemy | category_bits::kCliffMarker: {
      b2Fixture* enemyFixture = GetTargetFixture(category_bits::kEnemy, fixtureA, fixtureB);
      if (enemyFixture) {
        static_cast<Enemy*>(enemyFixture->GetUserData())->reverseDirection();
      }
      break;
    }
    // Set enemy as player's current target (so player can inflict damage to enemy).
    case category_bits::kMeleeWeapon | category_bits::kEnemy: {
      b2Fixture* weaponFixture = GetTargetFixture(category_bits::kMeleeWeapon, fixtureA, fixtureB);
      b2Fixture* enemyFixture = GetTargetFixture(category_bits::kEnemy, fixtureA, fixtureB);

      if (weaponFixture && enemyFixture) {
        Player* player = static_cast<Player*>(weaponFixture->GetUserData());
        Enemy* enemy = static_cast<Enemy*>(enemyFixture->GetUserData());
        player->getInRangeTargets().insert(enemy);
        //enemy->setIsAlerted(true);
      }
      break;
    }
    // Set player as enemy's current target (so enemy can inflict damage to player).
    case category_bits::kMeleeWeapon | category_bits::kPlayer: {
      b2Fixture* weaponFixture = GetTargetFixture(category_bits::kMeleeWeapon, fixtureA, fixtureB);
      b2Fixture* playerFixture = GetTargetFixture(category_bits::kPlayer, fixtureA, fixtureB);

      if (weaponFixture && playerFixture) {
        Enemy* enemy = static_cast<Enemy*>(weaponFixture->GetUserData());
        Player* player = static_cast<Player*>(playerFixture->GetUserData());
        enemy->getInRangeTargets().insert(player);
      }
      break;
    }
    default:
      break;
  }
}

void WorldContactListener::EndContact(b2Contact* contact) {
  b2Fixture* fixtureA = contact->GetFixtureA();
  b2Fixture* fixtureB = contact->GetFixtureB();

  int cDef = fixtureA->GetFilterData().categoryBits | fixtureB->GetFilterData().categoryBits;
  switch (cDef) {
    // When a character leaves the ground, make following changes.
    case category_bits::kFeet | category_bits::kGround: {
      b2Fixture* feetFixture = GetTargetFixture(category_bits::kFeet, fixtureA, fixtureB);
      if (feetFixture && feetFixture->GetBody()->GetLinearVelocity().y > .5f) {
        static_cast<Character*>(feetFixture->GetUserData())->setIsJumping(true);
      }
      break;
    }
    // When a character leaves the platform, make following changes.
    case category_bits::kFeet | category_bits::kPlatform: {
      b2Fixture* feetFixture = GetTargetFixture(category_bits::kFeet, fixtureA, fixtureB);
      if (feetFixture && feetFixture->GetBody()->GetLinearVelocity().y < -.5f) {
        Character* c = static_cast<Character*>(feetFixture->GetUserData());
        c->setIsJumping(true);
        c->setIsOnPlatform(false);
      }
      break;
    }
    // Clear player's current target (so player cannot inflict damage to enemy from a distance).
    case category_bits::kMeleeWeapon | category_bits::kEnemy: {
      b2Fixture* weaponFixture = GetTargetFixture(category_bits::kMeleeWeapon, fixtureA, fixtureB);
      b2Fixture* enemyFixture = GetTargetFixture(category_bits::kEnemy, fixtureA, fixtureB);

      if (weaponFixture && enemyFixture) {
        Player* player = static_cast<Player*>(weaponFixture->GetUserData());
        Enemy* enemy = static_cast<Enemy*>(enemyFixture->GetUserData());
        player->getInRangeTargets().erase(enemy);
      }
      break;
    }
    // Clear enemy's current target (so enemy cannot inflict damage to player from a distance).
    case category_bits::kMeleeWeapon | category_bits::kPlayer: {
      b2Fixture* weaponFixture = GetTargetFixture(category_bits::kMeleeWeapon, fixtureA, fixtureB);
      b2Fixture* playerFixture = GetTargetFixture(category_bits::kPlayer, fixtureA, fixtureB);

      if (weaponFixture && playerFixture) {
        Enemy* enemy = static_cast<Enemy*>(weaponFixture->GetUserData());
        Player* player = static_cast<Player*>(playerFixture->GetUserData());
        enemy->getInRangeTargets().erase(player);
      }
      break;
    }
    default:
      break;
  }
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
