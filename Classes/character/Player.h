// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_PLAYER_H_
#define VIGILANTE_PLAYER_H_

#include <string>

#include <Box2D/Box2D.h>
#include "Character.h"
#include "Controllable.h"
#include "item/Equipment.h"
#include "quest/QuestBook.h"

namespace vigilante {

class Player : public Character, public Controllable {
 public:
  explicit Player(const std::string& jsonFileName);
  virtual ~Player() = default;

  virtual void showOnMap(float x, float y) override; // Character
  virtual void removeFromMap() override; // Character
  virtual void handleInput() override; // Controllable

  virtual void inflictDamage(Character* target, int damage) override; // Character
  virtual void receiveDamage(Character* source, int damage) override; // Character

  virtual void equip(Equipment* equipment) override; // Character
  virtual void unequip(Equipment::Type equipmentType) override; // Character
  virtual void pickupItem(Item* item) override; // Character

  QuestBook& getQuestBook();

 private:
  QuestBook _questBook;
};

} // namespace vigilante

#endif // VIGILANTE_PLAYER_H_
