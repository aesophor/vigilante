#ifndef VIGILANTE_ENEMY_H_
#define VIGILANTE_ENEMY_H_

#include <string>

#include "Box2D/Box2D.h"

#include "Character.h"
#include "Bot.h"


namespace vigilante {

class Enemy : public Character, public Bot {
 public:
  Enemy(const std::string& name, float x, float y);
  virtual ~Enemy() = default;
  virtual void update(float delta) override;

  virtual void receiveDamage(Character* source, int damage) override;
};

} // namespace vigilante

#endif // VIGILANTE_ENEMY_H_
