// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_HUD_H_
#define VIGILANTE_HUD_H_

#include <memory>
#include <string>

#include <axmol.h>
#include <2d/Label.h>
#include <ui/UIImageView.h>

#include "StatusBar.h"

namespace vigilante {

class Hud final {
 public:
  Hud();

  void updateEquippedWeapon();
  void updateStatusBars();

  inline ax::Layer* getLayer() const { return _layer; }

 private:
  static inline constexpr float _kBarLength = 75.0f;

  ax::Layer* _layer{};
  std::unique_ptr<StatusBar> _healthBar;
  std::unique_ptr<StatusBar> _magickaBar;
  std::unique_ptr<StatusBar> _staminaBar;
  ax::ui::ImageView* _equippedWeaponBg{};
  ax::ui::ImageView* _equippedWeapon{};
  ax::ui::ImageView* _equippedWeaponDescBg{};
  ax::Label* _equippedWeaponDesc{};
};

}  // namespace vigilante

#endif  // VIGILANTE_HUD_H_
