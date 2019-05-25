#ifndef VIGILANTE_DYNAMIC_ACTOR_H_
#define VIGILANTE_DYNAMIC_ACTOR_H_

#include <vector>

#include "Box2D/Box2D.h"

#include "StaticActor.h"

namespace vigilante {

// A dynamic actor is an abstract class which represents a game entity
// consisting of the following members:
// 1. a b2Body (with one or more b2Fixtures attached to it)
// 2. a sprite synchronized with its b2Body
// 3. a spritesheet and several body animations
//
// If you need more sprites and animations (e.g., equipment displayed on top of a character),
// then manually subclass "Actor" and declare them as the members of your subclass.
//
// Notes on the pure virtual methods:
// 1. Actor::update(float delta) should "sync the sprites with its b2Body".
// 2. <insert next item...>

class DynamicActor : public StaticActor {
 public:
  DynamicActor(size_t animationsSize=1, size_t fixturesSize=1);
  virtual ~DynamicActor() = default;

  virtual void update(float delta) = 0;
  virtual void showOnMap(float x, float y) = 0; // StaticActor
  virtual void removeFromMap() override; // StaticActor

  b2Body* getBody() const;
  std::vector<b2Fixture*>& getFixtures();

 protected:
  b2Body* _body; // users should manually destory _body in subclass!
  std::vector<b2Fixture*> _fixtures;
};

} // namespace vigilante

#endif // VIGILANTE_DYNAMIC_ACTOR_H_
