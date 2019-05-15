#ifndef VIGILANTE_PAUSE_MENU_H_
#define VIGILANTE_PAUSE_MENU_H_

#include <vector>
#include <string>
#include <memory>

#include "cocos2d.h"
#include "2d/CCLabel.h"
#include "ui/UIImageView.h"

#include "HeaderPane.h"
#include "StatsPane.h"
#include "PauseMenuDialog.h"
#include "AbstractPane.h"
#include "character/Character.h"

namespace vigilante {

class HeaderPane;
class StatsPane;
class PauseMenuDialog;

class PauseMenu {
 public:
  PauseMenu(Character* character);
  virtual ~PauseMenu() = default;

  void update();
  void handleInput();

  Character* getCharacter() const;
  void setCharacter(Character* character);

  AbstractPane* getCurrentPane() const;
  cocos2d::Layer* getLayer() const;
  PauseMenuDialog* getDialog() const;

 private:
  Character* _character;

  cocos2d::Layer* _layer;
  cocos2d::ui::ImageView* _background;

  std::unique_ptr<HeaderPane> _headerPane;
  std::unique_ptr<StatsPane> _statsPane;
  std::unique_ptr<PauseMenuDialog> _dialog;
  std::vector<std::unique_ptr<AbstractPane>> _panes;
};

} // namespace vigilante

#endif // VIGILANTE_PAUSE_MENU_H_
