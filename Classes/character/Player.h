#ifndef VIGILANTE_PLAYER_H_
#define VIGILANTE_PLAYER_H_

#include <string>

#include "Box2D/Box2D.h"

#include "Character.h"


namespace vigilante {

class Player : public Character {
 public:
  Player(const std::string& name, float x, float y);
  virtual ~Player();

  virtual void inflictDamage(Character* target, int damage) override;
  virtual void receiveDamage(Character* source, int damage) override;

 private:
  virtual void defineTexture(float x, float y) override;
};

} // namespace vigilante

#endif // VIGILANTE_PLAYER_H_
