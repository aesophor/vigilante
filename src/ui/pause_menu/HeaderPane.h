// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_HEADER_PANE_H_
#define VIGILANTE_HEADER_PANE_H_

#include <string>
#include <vector>

#include <cocos2d.h>
#include <2d/CCLabel.h>
#include <ui/UILayout.h>

#include "ui/pause_menu/AbstractPane.h"

namespace vigilante {

class HeaderPane : public AbstractPane {
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

  std::vector<cocos2d::Label*> _labels;
  int _currentIndex;
};

} // namespace vigilante

#endif // VIGILANTE_HEADER_PANE_H_
