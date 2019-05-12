#ifndef VIGILANTE_PAUSE_MENU_H_
#define VIGILANTE_PAUSE_MENU_H_

#include <string>

#include "cocos2d.h"
#include "ui/UIImageView.h"

namespace vigilante {

class PauseMenu {
 public:
  PauseMenu();
  virtual ~PauseMenu() = default;

  cocos2d::Layer* getLayer() const;

 private:
  // Paths to resource files.
  static const std::string _kPauseMenu;

  static const std::string _kBackground;

  cocos2d::Layer* _layer;
  
  cocos2d::ui::ImageView* _background;
};

} // namespace vigilante

#endif // VIGILANTE_PAUSE_MENU_H_
