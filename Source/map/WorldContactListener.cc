// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "WorldContactListener.h"

#include <limits>

#include <axmol.h>

#include "CallbackManager.h"
#include "Constants.h"
#include "Projectile.h"
#include "character/Character.h"
#include "character/Player.h"
#include "character/Npc.h"
#include "item/Item.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "skill/Skill.h"
#include "skill/MagicalMissile.h"
#include "skill/ForwardSlash.h"
#include "util/Logger.h"

using namespace std;
USING_NS_AX;

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
        Character* c = reinterpret_cast<Character*>(feetFixture->GetUserData().pointer);
        c->setJumping(false);
        c->setDoubleJumping(false);
        c->setOnPlatform(false);
        c->onFallToGroundOrPlatform();

        auto fxMgr = SceneManager::the().getCurrentScene<GameScene>()->getFxManager();
        fxMgr->createDustFx(c);
      }
      break;
    }
    // When a character lands on a platform, make following changes.
    case category_bits::kFeet | category_bits::kPlatform: {
      b2Fixture* feetFixture = GetTargetFixture(category_bits::kFeet, fixtureA, fixtureB);
      if (feetFixture && feetFixture->GetBody()->GetLinearVelocity().y < -.01f) {
        Character* c = reinterpret_cast<Character*>(feetFixture->GetUserData().pointer);
        c->setJumping(false);
        c->setDoubleJumping(false);
        c->setOnPlatform(true);
        c->onFallToGroundOrPlatform();

        auto fxMgr = SceneManager::the().getCurrentScene<GameScene>()->getFxManager();
        fxMgr->createDustFx(c);
      }
      break;
    }
    // When a player bumps into an enemy, the enemy will inflict damage to the player and knock it back.
    case category_bits::kPlayer | category_bits::kEnemy: {
      b2Fixture* playerFixture = GetTargetFixture(category_bits::kPlayer, fixtureA, fixtureB);
      b2Fixture* enemyFixture = GetTargetFixture(category_bits::kEnemy, fixtureA, fixtureB);

      if (playerFixture && enemyFixture) {
        Character* player = reinterpret_cast<Character*>(playerFixture->GetUserData().pointer);
        Character* enemy = reinterpret_cast<Character*>(enemyFixture->GetUserData().pointer);

        if (!player->isInvincible()) {
          float knockBackForceX = (player->isFacingRight()) ? -.25f : .25f; // temporary
          float knockBackForceY = 1.0f; // temporary
          enemy->inflictDamage(player, 25);
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
        Character* npc = reinterpret_cast<Character*>(npcFixture->GetUserData().pointer);
        Character* enemy = reinterpret_cast<Character*>(enemyFixture->GetUserData().pointer);

        if (!npc->isInvincible()) {
          float knockBackForceX = (npc->isFacingRight()) ? -.25f : .25f; // temporary
          float knockBackForceY = 1.0f; // temporary
          enemy->inflictDamage(npc, 25);
          enemy->knockBack(npc, knockBackForceX, knockBackForceY);
        }
      }
      break;
    }

    case category_bits::kEnemy | category_bits::kPivotMarker: {
      b2Fixture* enemyFixture = GetTargetFixture(category_bits::kEnemy, fixtureA, fixtureB);
      if (enemyFixture) {
        reinterpret_cast<Npc*>(enemyFixture->GetUserData().pointer)->reverseDirection();
      }
      break;
    }
    case category_bits::kEnemy | category_bits::kCliffMarker: {
      b2Fixture* bodyFixture = GetTargetFixture(category_bits::kEnemy, fixtureA, fixtureB);
      if (bodyFixture) {
        reinterpret_cast<Character*>(bodyFixture->GetUserData().pointer)->doubleJump();
      }
      break;
    }
    case category_bits::kNpc | category_bits::kCliffMarker: {
      b2Fixture* bodyFixture = GetTargetFixture(category_bits::kNpc, fixtureA, fixtureB);
      if (bodyFixture) {
        reinterpret_cast<Character*>(bodyFixture->GetUserData().pointer)->doubleJump();
      }
      break;
    }

    // Set enemy as player's current target (so player can inflict damage to enemy).
    case category_bits::kMeleeWeapon | category_bits::kEnemy: {
      b2Fixture* weaponFixture = GetTargetFixture(category_bits::kMeleeWeapon, fixtureA, fixtureB);
      b2Fixture* enemyFixture = GetTargetFixture(category_bits::kEnemy, fixtureA, fixtureB);

      if (weaponFixture && enemyFixture) {
        Character* attacker = reinterpret_cast<Character*>(weaponFixture->GetUserData().pointer);
        Character* enemy = reinterpret_cast<Character*>(enemyFixture->GetUserData().pointer);
        attacker->getInRangeTargets().insert(enemy);

        // If player is using skill (e.g., forward slash), than inflict damage
        // when an enemy contacts player's weapon fixture.
        if (attacker->isUsingSkill() && dynamic_cast<ForwardSlash*>(attacker->getCurrentlyUsedSkill())) {
          int skillDmg = attacker->getCurrentlyUsedSkill()->getSkillProfile().physicalDamage;
          attacker->inflictDamage(enemy, attacker->getDamageOutput() + skillDmg);
        }
      }
      break;
    }
    // Set player as enemy's current target (so enemy can inflict damage to player).
    case category_bits::kMeleeWeapon | category_bits::kPlayer: {
      b2Fixture* weaponFixture = GetTargetFixture(category_bits::kMeleeWeapon, fixtureA, fixtureB);
      b2Fixture* playerFixture = GetTargetFixture(category_bits::kPlayer, fixtureA, fixtureB);

      if (weaponFixture && playerFixture) {
        Character* attacker = reinterpret_cast<Character*>(weaponFixture->GetUserData().pointer);
        Character* player = reinterpret_cast<Character*>(playerFixture->GetUserData().pointer);
        attacker->getInRangeTargets().insert(player);
      }
      break;
    }
    // Set Npc as enemy's current target (so enemy can inflict damage to the Npc).
    case category_bits::kMeleeWeapon | category_bits::kNpc: {
      b2Fixture* weaponFixture = GetTargetFixture(category_bits::kMeleeWeapon, fixtureA, fixtureB);
      b2Fixture* npcFixture = GetTargetFixture(category_bits::kNpc, fixtureA, fixtureB);

      if (weaponFixture && npcFixture) {
        Character* attacker = reinterpret_cast<Character*>(weaponFixture->GetUserData().pointer);
        Character* npc = reinterpret_cast<Character*>(npcFixture->GetUserData().pointer);
        attacker->getInRangeTargets().insert(npc);
      }
      break;
    }
    // Add the item to character's _inRangeItems set (so they can pick them up).
    case category_bits::kFeet | category_bits::kItem: {
      b2Fixture* feetFixture = GetTargetFixture(category_bits::kFeet, fixtureA, fixtureB);
      b2Fixture* itemFixture = GetTargetFixture(category_bits::kItem, fixtureA, fixtureB);

      if (feetFixture && itemFixture) {
        Character* c = reinterpret_cast<Character*>(feetFixture->GetUserData().pointer);
        Item* i = reinterpret_cast<Item*>(itemFixture->GetUserData().pointer);
        c->getInRangeItems().insert(i);
      }
      break;
    }
    // When a character gets close to a portal, register it to the character.
    case category_bits::kFeet | category_bits::kPortal: {
      b2Fixture* feetFixture = GetTargetFixture(category_bits::kFeet, fixtureA, fixtureB);
      b2Fixture* portalFixture = GetTargetFixture(category_bits::kPortal, fixtureA, fixtureB);

      if (feetFixture && portalFixture) {
        Character* c = reinterpret_cast<Character*>(feetFixture->GetUserData().pointer);
        GameMap::Portal* p = reinterpret_cast<GameMap::Portal*>(portalFixture->GetUserData().pointer);
        c->setPortal(p);

        if (p->willInteractOnContact()) {
          CallbackManager::the().runAfter([=]() {
            c->interact(p);
          }, .1f);
        } else if (!p->willInteractOnContact() && dynamic_cast<Player*>(c)) {
          p->showHintUI();
        }
      }
      break;
    }
    // When a character gets close to an interactable object or NPC, register it to the character.
    case category_bits::kFeet | category_bits::kInteractable: {
      b2Fixture* feetFixture = GetTargetFixture(category_bits::kFeet, fixtureA, fixtureB);
      b2Fixture* interactableFixture = GetTargetFixture(category_bits::kInteractable, fixtureA, fixtureB);

      if (feetFixture && interactableFixture) {
        Character* c = reinterpret_cast<Character*>(feetFixture->GetUserData().pointer);
        Interactable* i = reinterpret_cast<Interactable*>(interactableFixture->GetUserData().pointer);

        if (i->willInteractOnContact()) {
          CallbackManager::the().runAfter([=]() {
            c->interact(i);
          }, .1f);
          return;
        }

        auto& interactables = c->getInRangeInteractables();
        if (interactables.size()) {
          (*interactables.begin())->hideHintUI();
        }
        interactables.insert(i);

        if (dynamic_cast<Player*>(c)) {
          (*interactables.begin())->showHintUI();
        }
      }
      break;
    }
    // When a project tile hits an enemy, play onHitAnimation and inflict damage.
    case category_bits::kProjectile | category_bits::kEnemy: {
      b2Fixture* projectileFixture = GetTargetFixture(category_bits::kProjectile, fixtureA, fixtureB);
      b2Fixture* enemyFixture = GetTargetFixture(category_bits::kEnemy, fixtureA, fixtureB);

      if (projectileFixture && enemyFixture) {
        DynamicActor* p = reinterpret_cast<DynamicActor*>(projectileFixture->GetUserData().pointer);
        Character* c = reinterpret_cast<Character*>(enemyFixture->GetUserData().pointer);

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
        Character* c = reinterpret_cast<Character*>(feetFixture->GetUserData().pointer);

        auto fxMgr = SceneManager::the().getCurrentScene<GameScene>()->getFxManager();
        fxMgr->createDustFx(c);
      }
      break;
    }
    // When a character leaves the platform, make following changes.
    case category_bits::kFeet | category_bits::kPlatform: {
      b2Fixture* feetFixture = GetTargetFixture(category_bits::kFeet, fixtureA, fixtureB);
      if (feetFixture && feetFixture->GetBody()->GetLinearVelocity().y < -.5f) {
        Character* c = reinterpret_cast<Character*>(feetFixture->GetUserData().pointer);
        c->setOnPlatform(false);

        auto fxMgr = SceneManager::the().getCurrentScene<GameScene>()->getFxManager();
        fxMgr->createDustFx(c);
      }
      break;
    }
    // Clear player's current target (so player cannot inflict damage to enemy from a distance).
    case category_bits::kMeleeWeapon | category_bits::kEnemy: {
      b2Fixture* weaponFixture = GetTargetFixture(category_bits::kMeleeWeapon, fixtureA, fixtureB);
      b2Fixture* enemyFixture = GetTargetFixture(category_bits::kEnemy, fixtureA, fixtureB);

      if (weaponFixture && enemyFixture) {
        Character* attacker = reinterpret_cast<Character*>(weaponFixture->GetUserData().pointer);
        Character* enemy = reinterpret_cast<Character*>(enemyFixture->GetUserData().pointer);
        attacker->getInRangeTargets().erase(enemy);
      }
      break;
    }
    // Clear enemy's current target (so enemy cannot inflict damage to player from a distance).
    case category_bits::kMeleeWeapon | category_bits::kPlayer: {
      b2Fixture* weaponFixture = GetTargetFixture(category_bits::kMeleeWeapon, fixtureA, fixtureB);
      b2Fixture* playerFixture = GetTargetFixture(category_bits::kPlayer, fixtureA, fixtureB);

      if (weaponFixture && playerFixture) {
        Character* attacker = reinterpret_cast<Character*>(weaponFixture->GetUserData().pointer);
        Character* player = reinterpret_cast<Character*>(playerFixture->GetUserData().pointer);
        attacker->getInRangeTargets().erase(player);
      }
      break;
    }
    // Clear enemy's current target (so enemy cannot inflict damage to npc from a distance).
    case category_bits::kMeleeWeapon | category_bits::kNpc: {
      b2Fixture* weaponFixture = GetTargetFixture(category_bits::kMeleeWeapon, fixtureA, fixtureB);
      b2Fixture* npcFixture = GetTargetFixture(category_bits::kNpc, fixtureA, fixtureB);

      if (weaponFixture && npcFixture) {
        Character* attacker = reinterpret_cast<Character*>(weaponFixture->GetUserData().pointer);
        Character* npc = reinterpret_cast<Character*>(npcFixture->GetUserData().pointer);
        attacker->getInRangeTargets().erase(npc);
      }
      break;
    }
    // Remove the item from character's _inRangeItems set.
    case category_bits::kFeet | category_bits::kItem: {
      b2Fixture* feetFixture = GetTargetFixture(category_bits::kFeet, fixtureA, fixtureB);
      b2Fixture* itemFixture = GetTargetFixture(category_bits::kItem, fixtureA, fixtureB);

      if (feetFixture && itemFixture) {
        Character* c = reinterpret_cast<Character*>(feetFixture->GetUserData().pointer);
        Item* i = reinterpret_cast<Item*>(itemFixture->GetUserData().pointer);
        c->getInRangeItems().erase(i);
      }
      break;
    }
    // When a character leaves an interactable object, clear it from the character.
    case category_bits::kFeet | category_bits::kPortal: {
      b2Fixture* feetFixture = GetTargetFixture(category_bits::kFeet, fixtureA, fixtureB);
      b2Fixture* portalFixture = GetTargetFixture(category_bits::kPortal, fixtureA, fixtureB);

      if (feetFixture && portalFixture) {
        Character* c = reinterpret_cast<Character*>(feetFixture->GetUserData().pointer);
        GameMap::Portal* p = reinterpret_cast<GameMap::Portal*>(portalFixture->GetUserData().pointer);
        c->setPortal(nullptr);
        p->hideHintUI();
      }
      break;
    }
    // When a character leaves an interactable object, clear it from the character.
    case category_bits::kFeet | category_bits::kInteractable: {
      b2Fixture* feetFixture = GetTargetFixture(category_bits::kFeet, fixtureA, fixtureB);
      b2Fixture* interactableFixture = GetTargetFixture(category_bits::kInteractable, fixtureA, fixtureB);

      if (feetFixture && interactableFixture) {
        Character* c = reinterpret_cast<Character*>(feetFixture->GetUserData().pointer);
        Interactable* i = reinterpret_cast<Interactable*>(interactableFixture->GetUserData().pointer);
        c->getInRangeInteractables().erase(i);
        i->hideHintUI();
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
      b2Fixture* feetFixture = GetTargetFixture(category_bits::kFeet, fixtureA, fixtureB);
      b2Fixture* platform = GetTargetFixture(category_bits::kPlatform, fixtureA, fixtureB);

      const auto& playerPos = feetFixture->GetBody()->GetPosition();
      const auto& platformPos = platform->GetBody()->GetPosition();

      auto platformShape = static_cast<b2PolygonShape*>(platform->GetShape());
      float platformMinX = numeric_limits<float>::max();
      float platformMaxX = numeric_limits<float>::lowest();
      for (int i = 0; i < 4; i++) {
        b2Vec2 point = platformShape->m_vertices[i];
        point.x += platformPos.x;
        point.y += platformPos.y;

        platformMinX = std::min(platformMinX, point.x);
        platformMaxX = std::max(platformMaxX, point.x);
      }

      // Enable contact if the player is about to land on the platform.
      constexpr float kOffset = .18f;
      contact->SetEnabled(playerPos.x > platformMinX &&
                          playerPos.x < platformMaxX &&
                          playerPos.y > platformPos.y + kOffset);
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

}  // namespace vigilante
