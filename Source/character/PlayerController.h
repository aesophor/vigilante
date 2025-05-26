// Copyright (c) 2023-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_CHARACTER_PLAYER_CONTROLLER_H_
#define REQUIEM_CHARACTER_PLAYER_CONTROLLER_H_

#include "Controllable.h"

namespace requiem {

class Player;

class PlayerController final : public Controllable {
 public:
  explicit PlayerController(Player& player) : _player{player} {}

  // Controllable
  virtual void handleInput() override;

 private:
  bool shouldIgnoreInput() const;
  void handleSheatheUnsheatheWeaponInput();
  void handleAttackInput();
  void handleHotkeyInput();

  Player& _player;
};

}  // namespace requiem

#endif  // REQUIEM_CHARACTER_PLAYER_CONTROLLER_H_
