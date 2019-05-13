#ifndef VIGILANTE_PAUSE_MENU_H_
#define VIGILANTE_PAUSE_MENU_H_

#include <array>
#include <string>
#include <memory>

#include "cocos2d.h"
#include "2d/CCLabel.h"
#include "ui/UIImageView.h"

#include "HeaderPane.h"
#include "StatsPane.h"

namespace vigilante {

class PauseMenu {
 public:
  PauseMenu();
  virtual ~PauseMenu() = default;

  void handleInput();

  cocos2d::Layer* getLayer() const;

 private:
  // Paths to resource files.
  static const std::string _kPauseMenu;
  static const std::string _kBackground;
  static const std::string _kStatsBg;
  static const std::string _kFont; // header options label font
  static const float _kFontSize; // header options label font size

  cocos2d::Layer* _layer;
  cocos2d::ui::ImageView* _background;

  std::unique_ptr<HeaderPane> _headerPane;
  std::unique_ptr<StatsPane> _statsPane;
};

} // namespace vigilante

#endif // VIGILANTE_PAUSE_MENU_H_
