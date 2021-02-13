// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_HUD_H_
#define VIGILANTE_HUD_H_

#include <string>
#include <memory>

#include <cocos2d.h>
#include <2d/CCLabel.h>
#include <ui/UIImageView.h>
#include "StatusBar.h"

namespace vigilante {

class Hud {
 public:
  static Hud* getInstance();
  virtual ~Hud() = default;

  void updateEquippedWeapon();
  void updateStatusBars();

  cocos2d::Layer* getLayer() const;

 private:
  Hud();

  static const float _kBarLength;

  cocos2d::Layer* _layer;
  std::unique_ptr<StatusBar> _healthBar;
  std::unique_ptr<StatusBar> _magickaBar;
  std::unique_ptr<StatusBar> _staminaBar;
  cocos2d::ui::ImageView* _equippedWeaponBg;
  cocos2d::ui::ImageView* _equippedWeapon;
  cocos2d::ui::ImageView* _equippedWeaponDescBg;
  cocos2d::Label* _equippedWeaponDesc;
};

}  // namespace vigilante

#endif  // VIGILANTE_HUD_H_
