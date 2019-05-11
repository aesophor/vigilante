#ifndef VIGILANTE_HUD_H_
#define VIGILANTE_HUD_H_

#include <string>
#include <memory>

#include "cocos2d.h"

#include "character/Player.h"
#include "StatusBar.h"

namespace vigilante {

class Hud {
 public:
  static Hud* getInstance();
  virtual ~Hud() = default;
  void update();

  cocos2d::Layer* getLayer() const;
  void setPlayer(Player* player);

 private:
  static Hud* _instance;
  Hud();

  static const std::string _kHudLocation;
  static const std::string _kBarLeftPaddingLocation;
  static const std::string _kBarRightPaddingLocation;
  static const std::string _kHealthBarLocation;
  static const std::string _kMagickaBarLocation;
  static const std::string _kStaminaBarLocation;
  static const float _kBarLength;

  cocos2d::Layer* _layer;
  Player* _player;

  std::unique_ptr<StatusBar> _healthBar;
  std::unique_ptr<StatusBar> _magickaBar;
  std::unique_ptr<StatusBar> _staminaBar;
};

} // namespace vigilante

#endif // VIGILANTE_HUD_H_
