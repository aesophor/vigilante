// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_UI_PAUSE_MENU_PAUSE_MENU_H_
#define VIGILANTE_UI_PAUSE_MENU_PAUSE_MENU_H_

#include <array>
#include <memory>
#include <string>
#include <vector>

#include <axmol.h>
#include <2d/Label.h>
#include <ui/UIImageView.h>

#include "Controllable.h"
#include "ui/hud/ControlHints.h"
#include "ui/pause_menu/HeaderPane.h"
#include "ui/pause_menu/StatsPane.h"
#include "ui/pause_menu/PauseMenuDialog.h"
#include "ui/pause_menu/AbstractPane.h"

namespace vigilante {

// Forward Declaration
class Player;
class HeaderPane;
class StatsPane;
class PauseMenuDialog;

class PauseMenu final : public Controllable {
 public:
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
  static const inline std::array<std::string, PauseMenu::Pane::SIZE> _kPaneNames = {{
    "INVENTORY",
    "EQUIPMENT",
    "SKILLS",
    "QUESTS",
    "OPTIONS"
  }};

  PauseMenu();
  virtual ~PauseMenu() override = default;

  void update();
  virtual void handleInput() override;
  void show(PauseMenu::Pane pane);

  AbstractPane* getCurrentPane() const;
  inline ax::Layer* getLayer() const { return _layer; }
  inline PauseMenuDialog* getDialog() const { return _dialog.get(); }
  Player* getPlayer() const;

  inline bool isVisible() const { return _layer->isVisible(); }
  void setVisible(bool visible);

 private:
  void initMainPane(int index, std::unique_ptr<AbstractPane> pane);

  ax::Layer* _layer;
  ax::ui::ImageView* _background;

  std::unique_ptr<HeaderPane> _headerPane;
  std::unique_ptr<StatsPane> _statsPane;
  std::unique_ptr<PauseMenuDialog> _dialog;
  std::array<std::unique_ptr<AbstractPane>, PauseMenu::Pane::SIZE> _panes;
};

}  // namespace vigilante

#endif  // VIGILANTE_UI_PAUSE_MENU_PAUSE_MENU_H_
