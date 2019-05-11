#ifndef VIGILANTE_ENEMY_H_
#define VIGILANTE_ENEMY_H_

#include <string>

#include "Box2D/Box2D.h"

#include "Character.h"


namespace vigilante {

class Enemy : public Character {
 public:
  Enemy(const std::string& name, float x, float y);
  virtual ~Enemy();

 private:
  virtual void defineTexture(float x, float y) override;
};

} // namespace vigilante

#endif // VIGILANTE_ENEMY_H_
