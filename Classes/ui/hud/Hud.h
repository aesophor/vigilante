#ifndef VIGILANTE_HUD_H_
#define VIGILANTE_HUD_H_

#include <string>
#include <memory>

#include "cocos2d.h"
#include "ui/UIImageView.h"
#include "2d/CCLabel.h"

#include "character/Player.h"
#include "StatusBar.h"

namespace vigilante {

class Hud {
 public:
  static Hud* getInstance();
  virtual ~Hud() = default;

  void updateEquippedWeapon();
  void updateStatusBars();

  cocos2d::Layer* getLayer() const;
  void setPlayer(Player* player);

 private:
  static Hud* _instance;
  Hud();

  // Paths to HUD resources files
  static const std::string _kHud;

  static const std::string _kBarLeftPadding;
  static const std::string _kBarRightPadding;
  static const std::string _kHealthBar;
  static const std::string _kMagickaBar;
  static const std::string _kStaminaBar;
  static const float _kBarLength;

  static const std::string _kEquippedWeaponBg;
  static const std::string _kEquippedWeaponDescBg;
  static const std::string _kFont;

  cocos2d::Layer* _layer;
  Player* _player;

  std::unique_ptr<StatusBar> _healthBar;
  std::unique_ptr<StatusBar> _magickaBar;
  std::unique_ptr<StatusBar> _staminaBar;
  cocos2d::ui::ImageView* _equippedWeaponBg;
  cocos2d::ui::ImageView* _equippedWeapon;
  cocos2d::ui::ImageView* _equippedWeaponDescBg;
  cocos2d::Label* _equippedWeaponDesc;
};

} // namespace vigilante

#endif // VIGILANTE_HUD_H_
