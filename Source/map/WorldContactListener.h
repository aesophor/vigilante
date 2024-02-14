// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_MAP_WORLD_CONTACT_LISTENER_H_
#define VIGILANTE_MAP_WORLD_CONTACT_LISTENER_H_

#include <box2d/box2d.h>

namespace vigilante {

class WorldContactListener : public b2ContactListener {
 public:
  virtual void BeginContact(b2Contact* contact) override;
  virtual void EndContact(b2Contact* contact) override;
  virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
  virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

 private:
  b2Fixture* GetTargetFixture(short targetCategoryBits, b2Fixture* f1, b2Fixture* f2) const;
};

}  // namespace vigilante

#endif  // VIGILANTE_MAP_WORLD_CONTACT_LISTENER_H_
