// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_UI_PAUSE_MENU_HEADER_PANE_H_
#define VIGILANTE_UI_PAUSE_MENU_HEADER_PANE_H_

#include <string>
#include <vector>

#include <axmol.h>
#include <2d/Label.h>
#include <ui/UILayout.h>

#include "ui/pause_menu/AbstractPane.h"

namespace vigilante {

class HeaderPane final : public AbstractPane {
 public:
  explicit HeaderPane(PauseMenu* pauseMenu);
  virtual ~HeaderPane() = default;

  virtual void update() override;
  virtual void handleInput() override;

  void select(int index);
  void selectPrev();
  void selectNext();
  inline int getCurrentIndex() const { return _currentIndex; }

 private:
  enum Options {
    INVENTORY,
    EQUIPMENT,
    SKILLS,
    QUESTS,
    OPTIONS
  };

  static inline constexpr float _kOptionGap = 30.0f;
  static inline constexpr int _kOptionCount = 5;

  std::vector<ax::Label*> _labels;
  int _currentIndex{};
};

}  // namespace vigilante

#endif  // VIGILANTE_UI_PAUSE_MENU_HEADER_PANE_H_
