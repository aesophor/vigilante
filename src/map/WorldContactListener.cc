// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "WorldContactListener.h"

#include <cocos2d.h>
#include "Constants.h"
#include "Projectile.h"
#include "character/Player.h"
#include "character/Npc.h"
#include "item/Item.h"
#include "map/GameMap.h"
#include "map/GameMapManager.h"
#include "skill/Skill.h"
#include "skill/MagicalMissile.h"
#include "skill/ForwardSlash.h"
#include "util/CallbackUtil.h"

using std::unique_ptr;

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
        Character* c = static_cast<Character*>(feetFixture->GetUserData());
        c->setJumping(false);
        c->setDoubleJumping(false);
        c->setOnPlatform(false);
        // Create dust effect.
        GameMapManager::getInstance()->createDustFx(c);
      }
      break;
    }
    // When a character lands on a platform, make following changes.
    case category_bits::kFeet | category_bits::kPlatform: {
      b2Fixture* feetFixture = GetTargetFixture(category_bits::kFeet, fixtureA, fixtureB);
      if (feetFixture) {
        Character* c = static_cast<Character*>(feetFixture->GetUserData());
        c->setJumping(false);
        c->setDoubleJumping(false);
        c->setOnPlatform(true);
        // Create dust effect.
        GameMapManager::getInstance()->createDustFx(c);
      }
      break;
    }
    // When a player bumps into an enemy, the enemy will inflict damage to the player and knock it back.
    case category_bits::kPlayer | category_bits::kEnemy: {
      b2Fixture* playerFixture = GetTargetFixture(category_bits::kPlayer, fixtureA, fixtureB);
      b2Fixture* enemyFixture = GetTargetFixture(category_bits::kEnemy, fixtureA, fixtureB);

      if (playerFixture && enemyFixture) {
        Player* player = static_cast<Player*>(playerFixture->GetUserData());
        Npc* enemy = static_cast<Npc*>(enemyFixture->GetUserData());

        if (!player->isInvincible()) {
          enemy->inflictDamage(player, 25);
          float knockBackForceX = (player->isFacingRight()) ? -.25f : .25f; // temporary
          float knockBackForceY = 1.0f; // temporary
          enemy->knockBack(player, knockBackForceX, knockBackForceY);
        }
      }
      break;
    }
    // When an ally Npc bumps into an enemy, the enemy will inflict damage to the ally and knock it back.
    case category_bits::kNpc | category_bits::kEnemy: {
      b2Fixture* npcFixture = GetTargetFixture(category_bits::kNpc, fixtureA, fixtureB);
      b2Fixture* enemyFixture = GetTargetFixture(category_bits::kEnemy, fixtureA, fixtureB);

      if (npcFixture && enemyFixture) {
        Npc* npc = static_cast<Npc*>(npcFixture->GetUserData());
        Npc* enemy = static_cast<Npc*>(enemyFixture->GetUserData());

        if (!npc->isInvincible()) {
          enemy->inflictDamage(npc, 25);
          float knockBackForceX = (npc->isFacingRight()) ? -.25f : .25f; // temporary
          float knockBackForceY = 1.0f; // temporary
          enemy->knockBack(npc, knockBackForceX, knockBackForceY);
        }
      }
      break;
    }

    case category_bits::kEnemy | category_bits::kPivotMarker: {
      b2Fixture* enemyFixture = GetTargetFixture(category_bits::kEnemy, fixtureA, fixtureB);
      if (enemyFixture) {
        static_cast<Npc*>(enemyFixture->GetUserData())->reverseDirection();
      }
      break;
    }
    case category_bits::kEnemy | category_bits::kCliffMarker: {
      b2Fixture* bodyFixture = GetTargetFixture(category_bits::kEnemy, fixtureA, fixtureB);
      if (bodyFixture) {
        static_cast<Character*>(bodyFixture->GetUserData())->doubleJump();
      }
      break;
    }
    case category_bits::kNpc | category_bits::kCliffMarker: {
      b2Fixture* bodyFixture = GetTargetFixture(category_bits::kNpc, fixtureA, fixtureB);
      if (bodyFixture) {
        static_cast<Character*>(bodyFixture->GetUserData())->doubleJump();
      }
      break;
    }

    // Set enemy as player's current target (so player can inflict damage to enemy).
    case category_bits::kMeleeWeapon | category_bits::kEnemy: {
      b2Fixture* weaponFixture = GetTargetFixture(category_bits::kMeleeWeapon, fixtureA, fixtureB);
      b2Fixture* enemyFixture = GetTargetFixture(category_bits::kEnemy, fixtureA, fixtureB);

      if (weaponFixture && enemyFixture) {
        Player* player = static_cast<Player*>(weaponFixture->GetUserData());
        Npc* enemy = static_cast<Npc*>(enemyFixture->GetUserData());
        player->getInRangeTargets().insert(enemy);

        // If player is using skill (e.g., forward slash), than inflict damage
        // when an enemy contacts player's weapon fixture.
        if (player->isUsingSkill() && dynamic_cast<ForwardSlash*>(player->getCurrentlyUsedSkill())) {
          int skillDmg = player->getCurrentlyUsedSkill()->getSkillProfile().physicalDamage;
          player->inflictDamage(enemy, player->getDamageOutput() + skillDmg);
        }
      }
      break;
    }
    // Set player as enemy's current target (so enemy can inflict damage to player).
    case category_bits::kMeleeWeapon | category_bits::kPlayer: {
      b2Fixture* weaponFixture = GetTargetFixture(category_bits::kMeleeWeapon, fixtureA, fixtureB);
      b2Fixture* playerFixture = GetTargetFixture(category_bits::kPlayer, fixtureA, fixtureB);

      if (weaponFixture && playerFixture) {
        Npc* enemy = static_cast<Npc*>(weaponFixture->GetUserData());
        Player* player = static_cast<Player*>(playerFixture->GetUserData());
        enemy->getInRangeTargets().insert(player);
      }
      break;
    }
    // Add the item to character's _inRangeItems set (so they can pick them up).
    case category_bits::kFeet | category_bits::kItem: {
      b2Fixture* feetFixture = GetTargetFixture(category_bits::kFeet, fixtureA, fixtureB);
      b2Fixture* itemFixture = GetTargetFixture(category_bits::kItem, fixtureA, fixtureB);

      if (feetFixture && itemFixture) {
        Character* c = static_cast<Character*>(feetFixture->GetUserData());
        Item* i = static_cast<Item*>(itemFixture->GetUserData());
        c->getInRangeItems().insert(i);
      }
      break;
    }
    // When a character gets close to a portal, register it to the character.
    case category_bits::kFeet | category_bits::kPortal: {
      b2Fixture* feetFixture = GetTargetFixture(category_bits::kFeet, fixtureA, fixtureB);
      b2Fixture* portalFixture = GetTargetFixture(category_bits::kPortal, fixtureA, fixtureB);
      
      if (feetFixture && portalFixture) {
        Character* c = static_cast<Character*>(feetFixture->GetUserData());
        GameMap::Portal* p = static_cast<GameMap::Portal*>(portalFixture->GetUserData());
        c->setPortal(p);

        if (p->willInteractOnContact()) {
          callback_util::runAfter([=]() {
            c->interact(p);
          }, .1f);
        }
      }
      break;
    }
    // When a character gets close to an interactable object or NPC, register it to the character.
    case category_bits::kFeet | category_bits::kInteractableObject: {
      b2Fixture* feetFixture = GetTargetFixture(category_bits::kFeet, fixtureA, fixtureB);
      b2Fixture* objFixture = GetTargetFixture(category_bits::kInteractableObject, fixtureA, fixtureB);
      
      if (feetFixture && objFixture) {
        Character* c = static_cast<Character*>(feetFixture->GetUserData());
        Interactable* obj = static_cast<Interactable*>(objFixture->GetUserData());
        c->setInteractableObject(obj);

        if (obj->willInteractOnContact()) {
          callback_util::runAfter([=]() {
            c->interact(obj);
          }, .1f);
        }
      }
      break;
    }
    // When a character gets close to an NPC, register it to the character.
    case category_bits::kFeet | category_bits::kNpc: {
      b2Fixture* feetFixture = GetTargetFixture(category_bits::kFeet, fixtureA, fixtureB);
      b2Fixture* npcFixture = GetTargetFixture(category_bits::kNpc, fixtureA, fixtureB);
      
      if (feetFixture && npcFixture) {
        Character* c = static_cast<Character*>(feetFixture->GetUserData());
        Npc* npc = static_cast<Npc*>(npcFixture->GetUserData());
        c->setInteractableObject(npc);

        if (npc->willInteractOnContact()) {
          callback_util::runAfter([=]() {
            c->interact(npc);
          }, .1f);
        }
      }
      break;
    }
    // When a project tile hits an enemy, play onHitAnimation and inflict damage.
    case category_bits::kProjectile | category_bits::kEnemy: {
      b2Fixture* projectileFixture = GetTargetFixture(category_bits::kProjectile, fixtureA, fixtureB);
      b2Fixture* enemyFixture = GetTargetFixture(category_bits::kEnemy, fixtureA, fixtureB);
      
      if (projectileFixture && enemyFixture) {
        DynamicActor* p = static_cast<DynamicActor*>(projectileFixture->GetUserData());
        Character* c = static_cast<Character*>(enemyFixture->GetUserData());

        Projectile* missile = dynamic_cast<Projectile*>(p);
        missile->onHit(c);
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
        Character* c = static_cast<Character*>(feetFixture->GetUserData());
        c->setJumping(true);
        // Create dust effect.
        GameMapManager::getInstance()->createDustFx(c);
      }
      break;
    }
    // When a character leaves the platform, make following changes.
    case category_bits::kFeet | category_bits::kPlatform: {
      b2Fixture* feetFixture = GetTargetFixture(category_bits::kFeet, fixtureA, fixtureB);
      if (feetFixture && feetFixture->GetBody()->GetLinearVelocity().y < -.5f) {
        Character* c = static_cast<Character*>(feetFixture->GetUserData());
        c->setJumping(true);
        c->setOnPlatform(false);
        // Create dust effect.
        GameMapManager::getInstance()->createDustFx(c);
      }
      break;
    }
    // Clear player's current target (so player cannot inflict damage to enemy from a distance).
    case category_bits::kMeleeWeapon | category_bits::kEnemy: {
      b2Fixture* weaponFixture = GetTargetFixture(category_bits::kMeleeWeapon, fixtureA, fixtureB);
      b2Fixture* enemyFixture = GetTargetFixture(category_bits::kEnemy, fixtureA, fixtureB);

      if (weaponFixture && enemyFixture) {
        Player* player = static_cast<Player*>(weaponFixture->GetUserData());
        Npc* enemy = static_cast<Npc*>(enemyFixture->GetUserData());
        player->getInRangeTargets().erase(enemy);
      }
      break;
    }
    // Clear enemy's current target (so enemy cannot inflict damage to player from a distance).
    case category_bits::kMeleeWeapon | category_bits::kPlayer: {
      b2Fixture* weaponFixture = GetTargetFixture(category_bits::kMeleeWeapon, fixtureA, fixtureB);
      b2Fixture* playerFixture = GetTargetFixture(category_bits::kPlayer, fixtureA, fixtureB);

      if (weaponFixture && playerFixture) {
        Npc* enemy = static_cast<Npc*>(weaponFixture->GetUserData());
        Player* player = static_cast<Player*>(playerFixture->GetUserData());
        enemy->getInRangeTargets().erase(player);
      }
      break;
    }
    // Remove the item from character's _inRangeItems set.
    case category_bits::kFeet | category_bits::kItem: {
      b2Fixture* feetFixture = GetTargetFixture(category_bits::kFeet, fixtureA, fixtureB);
      b2Fixture* itemFixture = GetTargetFixture(category_bits::kItem, fixtureA, fixtureB);

      if (feetFixture && itemFixture) {
        Character* c = static_cast<Character*>(feetFixture->GetUserData());
        Item* i = static_cast<Item*>(itemFixture->GetUserData());
        c->getInRangeItems().erase(i);
      }
      break;
    }
    // When a character leaves an interactable object, clear it from the character.
    case category_bits::kFeet | category_bits::kPortal: {
      b2Fixture* feetFixture = GetTargetFixture(category_bits::kFeet, fixtureA, fixtureB);
      b2Fixture* portalFixture = GetTargetFixture(category_bits::kPortal, fixtureA, fixtureB);
      
      if (feetFixture && portalFixture) {
        Character* c = static_cast<Character*>(feetFixture->GetUserData());
        c->setPortal(nullptr);
      }
      break;
    }
    // When a character leaves an interactable object, clear it from the character.
    case category_bits::kFeet | category_bits::kInteractableObject: {
      b2Fixture* feetFixture = GetTargetFixture(category_bits::kFeet, fixtureA, fixtureB);
      b2Fixture* objFixture = GetTargetFixture(category_bits::kInteractableObject, fixtureA, fixtureB);
      
      if (feetFixture && objFixture) {
        Character* c = static_cast<Character*>(feetFixture->GetUserData());
        c->setInteractableObject(nullptr);
      }
      break;
    }
    // When a character leaves an NPC, clear it from the character.
    case category_bits::kFeet | category_bits::kNpc: {
      b2Fixture* feetFixture = GetTargetFixture(category_bits::kFeet, fixtureA, fixtureB);
      b2Fixture* npcFixture = GetTargetFixture(category_bits::kNpc, fixtureA, fixtureB);
      
      if (feetFixture && npcFixture) {
        Character* c = static_cast<Character*>(feetFixture->GetUserData());
        c->setInteractableObject(nullptr);
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
