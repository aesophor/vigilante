#ifndef VIGILANTE_PLAYER_H_
#define VIGILANTE_PLAYER_H_

#include <string>

#include "Box2D/Box2D.h"

#include "Character.h"
#include "Controllable.h"
#include "item/Equipment.h"

namespace vigilante {

class Player : public Character, public Controllable {
 public:
  Player(const std::string& jsonFileName);
  virtual ~Player() = default;

  virtual void showOnMap(float x, float y) override; // Character
  virtual void handleInput() override; // Controllable

  virtual void inflictDamage(Character* target, int damage) override; // Character
  virtual void receiveDamage(Character* source, int damage) override; // Character

  virtual void equip(Equipment* equipment) override; // Character
  virtual void unequip(Equipment::Type equipmentType) override; // Character
};

} // namespace vigilante

#endif // VIGILANTE_PLAYER_H_
