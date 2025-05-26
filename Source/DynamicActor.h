// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_DYNAMIC_ACTOR_H_
#define REQUIEM_DYNAMIC_ACTOR_H_

#include <vector>

#include <box2d/box2d.h>

#include "StaticActor.h"

namespace requiem {

// A dynamic actor is an abstract class which represents a game entity
// consisting of the following members:
// 1. a b2Body (with one or more b2Fixtures attached to it)
// 2. a sprite synchronized with its b2Body
// 3. a spritesheet and several body animations
//
// If you need more sprites and animations
// (e.g., equipment displayed on top of a character),
// then manually subclass "DynamicActor" and
// declare them as the members of your subclass.
class DynamicActor : public StaticActor {
 public:
  DynamicActor(const std::size_t numAnimations = 1, const std::size_t numFixtures = 1)
      : StaticActor{numAnimations},
        _fixtures(numFixtures) {}
  virtual ~DynamicActor() override = default;

  virtual bool showOnMap(float x, float y) override = 0;  // StaticActor
  virtual bool removeFromMap() override;  // StaticActor:
  virtual void setPosition(float x, float y) override;  // StaticActor
  virtual void update(const float delta);
  virtual void destroyBody();

  inline b2Body* getBody() const { return _body; }
  inline std::vector<b2Fixture*>& getFixtures() { return _fixtures; }

 protected:
  static void setCategoryBits(b2Fixture* fixture, const short categoryBits);
  static void setMaskBits(b2Fixture* fixture, const short maskBits);

  b2Body* _body{};  // users should manually destory _body in subclass!
  std::vector<b2Fixture*> _fixtures;
};

}  // namespace requiem

#endif  // REQUIEM_DYNAMIC_ACTOR_H_
