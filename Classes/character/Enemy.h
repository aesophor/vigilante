#ifndef VIGILANTE_ENEMY_H_
#define VIGILANTE_ENEMY_H_

#include <string>

#include "Box2D/Box2D.h"

#include "Character.h"
#include "Bot.h"

namespace vigilante {

class Item;

class Enemy : public Character, public Bot {
 public:
  Enemy(const std::string& jsonFileName, float x, float y);
  virtual ~Enemy() = default;
  virtual void update(float delta) override;

  virtual void receiveDamage(Character* source, int damage) override;
//  virtual void dropItem(Item* item);
};

} // namespace vigilante

#endif // VIGILANTE_ENEMY_H_
