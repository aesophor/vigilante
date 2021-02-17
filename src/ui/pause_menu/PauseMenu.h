// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_PAUSE_MENU_H_
#define VIGILANTE_PAUSE_MENU_H_

#include <array>
#include <vector>
#include <string>
#include <memory>

#include <cocos2d.h>
#include <2d/CCLabel.h>
#include <ui/UIImageView.h>
#include "Controllable.h"
#include "ui/control_hints/ControlHints.h"
#include "ui/pause_menu/HeaderPane.h"
#include "ui/pause_menu/StatsPane.h"
#include "ui/pause_menu/PauseMenuDialog.h"
#include "ui/pause_menu/AbstractPane.h"

namespace vigilante {

class Player;
class HeaderPane;
class StatsPane;
class PauseMenuDialog;

class PauseMenu : public Controllable {
 public:
  static PauseMenu* getInstance();

  // To add a new pane to the pause menu, add it to the enum below,
  // as well as the string literal to the following static const std::array.
  enum Pane {
    INVENTORY,
    EQUIPMENT,
    SKILLS,
    QUESTS,
    OPTIONS,
    SIZE
  };
  static const std::array<std::string, PauseMenu::Pane::SIZE> _kPaneNames;

  virtual ~PauseMenu() = default;

  void update();
  virtual void handleInput() override;
  void show(PauseMenu::Pane pane);

  Player* getPlayer() const;
  void setPlayer(Player* character);

  AbstractPane* getCurrentPane() const;
  cocos2d::Layer* getLayer() const;
  PauseMenuDialog* getDialog() const;

  bool isVisible() const;
  void setVisible(bool visible);

 private:
  PauseMenu();
  void initMainPane(int index, std::unique_ptr<AbstractPane> pane);

  cocos2d::Layer* _layer;
  cocos2d::ui::ImageView* _background;

  std::unique_ptr<HeaderPane> _headerPane;
  std::unique_ptr<StatsPane> _statsPane;
  std::unique_ptr<PauseMenuDialog> _dialog;
  std::array<std::unique_ptr<AbstractPane>, PauseMenu::Pane::SIZE> _panes;

  Player* _player;
};

}  // namespace vigilante

#endif  // VIGILANTE_PAUSE_MENU_H_
