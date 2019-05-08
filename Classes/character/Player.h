#ifndef VIGILANTE_PLAYER_H_
#define VIGILANTE_PLAYER_H_

#include "Box2D/Box2D.h"

#include "map/GameMapManager.h"

namespace vigilante {

class Player {
 public:
  Player(float x, float y);
  virtual ~Player();

  void moveLeft() const;
  void moveRight() const;

  b2Body* getB2Body() const;

 private:
  void defineBody(float x, float y);

  static const float kBaseMovingSpeed;

  b2Body* _b2body;
  b2Fixture* _bodyFixture;

  float _stateTimer;
  bool _isFacingRight;
  bool _isJumping;
  bool _isKilled;
  bool _isSetToKill;
};

} // namespace vigilante

#endif // VIGILANTE_PLAYER_H_
