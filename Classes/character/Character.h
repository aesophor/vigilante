#ifndef VIGILANTE_CHARACTER_H_
#define VIGILANTE_CHARACTER_H_

#include "Box2D/Box2D.h"

namespace vigilante {

class Character {
 public:
  virtual ~Character();

  enum class State {
    IDLE,
    RUNNING,
    JUMPING,
    FALLING,
    CROUCHING,
    ATTACKING,
    KILLED
  };

 protected:
  Character();

  Character::State _currentState;
  Character::State _previousState;

  b2Body* _b2body;
  b2Fixture _bodyFixture;
};

} // namespace vigilante

#endif // VIGILANTE_CHARACTER_H_
