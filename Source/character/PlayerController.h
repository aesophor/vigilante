// Copyright (c) 2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_PLAYER_CONTROLLER_H_
#define VIGILANTE_PLAYER_CONTROLLER_H_

#include "Controllable.h"

namespace vigilante {

class Player;

class PlayerController final : public Controllable {
 public:
  explicit PlayerController(Player& player) : _player{player} {}

  // Controllable
  virtual void handleInput() override;

 private:
  bool shouldBlockInput() const;
  void handleSheatheUnsheatheWeaponInput();
  void handleAttackInput();
  void handleHotkeyInput();

  Player& _player;
};

}  // namespace vigilante

#endif  // VIGILANTE_PLAYER_CONTROLLER_H_
