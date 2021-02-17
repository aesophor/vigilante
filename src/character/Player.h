// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_PLAYER_H_
#define VIGILANTE_PLAYER_H_

#include <memory>
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

  virtual bool showOnMap(float x, float y) override;  // Character
  
  virtual void inflictDamage(Character* target, int damage) override;  // Character
  virtual void receiveDamage(Character* source, int damage) override;  // Character

  virtual void addItem(std::shared_ptr<Item> item, int amount=1) override;  // Character
  virtual void removeItem(Item* item, int amount=1) override;  // Character
  virtual void equip(Equipment* equipment) override;  // Character
  virtual void unequip(Equipment::Type equipmentType) override;  // Character
  virtual void pickupItem(Item* item) override;  // Character
  virtual void addExp(const int exp) override;  // Character

  virtual void handleInput() override;  // Controllable


  void updateKillTargetObjectives(Character* killedCharacter);

  QuestBook& getQuestBook();

 private:
  QuestBook _questBook;
};

}  // namespace vigilante

#endif  // VIGILANTE_PLAYER_H_
