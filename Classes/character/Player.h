#ifndef VIGILANTE_PLAYER_H_
#define VIGILANTE_PLAYER_H_

#include "Box2D/Box2D.h"

#include "map/GameMapManager.h"

namespace vigilante {

class Player {
 public:
  Player(float x, float y);
  virtual ~Player();

 private:
  void defineBody(float x, float y);

  b2Fixture* _bodyFixture;
};

} // namespace vigilante

#endif // VIGILANTE_PLAYER_H_
