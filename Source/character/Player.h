// Copyright (c) 2018-2025 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_CHARACTER_PLAYER_H_
#define VIGILANTE_CHARACTER_PLAYER_H_

#include <memory>
#include <string>

#include <box2d/box2d.h>

#include "Assets.h"
#include "Controllable.h"
#include "character/Character.h"
#include "character/PlayerController.h"
#include "item/Equipment.h"
#include "quest/QuestBook.h"

namespace vigilante {

class Player final : public Character, public Controllable {
 public:
  explicit Player(const std::filesystem::path& jsonFilePath);
  virtual ~Player() override = default;

  virtual bool showOnMap(float x, float y) override;  // Character
  virtual void onKilled() override;  // Character

  virtual bool inflictDamage(Character* target, int damage) override;  // Character
  virtual bool receiveDamage(Character* source, int damage) override;  // Character

  virtual bool addItem(std::shared_ptr<Item> item, int amount=1) override;  // Character
  virtual bool removeItem(Item* item, int amount=1) override;  // Character
  virtual void equip(Equipment* equipment, bool audio = true) override;  // Character
  virtual void unequip(Equipment::Type equipmentType, bool audio = true) override;  // Character
  virtual void pickupItem(Item* item) override;  // Character
  virtual void interact(Interactable* target) override;  // Character
  virtual void addExp(const int exp) override;  // Character

  virtual void handleInput() override;  // Controllable

  void updateKillTargetObjectives(Character* killedCharacter);

  inline QuestBook& getQuestBook() { return _questBook; }

 private:
  PlayerController _playerController;
  QuestBook _questBook;
};

}  // namespace vigilante

#endif  // VIGILANTE_CHARACTER_PLAYER_H_
