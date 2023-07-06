// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_STATS_PANE_H_
#define VIGILANTE_STATS_PANE_H_

#include <string>

#include <axmol.h>
#include <2d/Label.h>
#include <ui/UILayout.h>
#include <ui/UIImageView.h>

#include "AbstractPane.h"
#include "ui/TableLayout.h"

namespace vigilante {

class StatsPane : public AbstractPane {
 public:
  explicit StatsPane(PauseMenu* pauseMenu);
  virtual ~StatsPane() = default;
  virtual void update() override;
  virtual void handleInput() override;

 private:
  void addEntry(const std::string& title, ax::Label* label) const;

  static inline constexpr float _kPadLeft = 10.0f;
  static inline constexpr float _kPadRight = 10.0f;
  static inline constexpr float _kRowHeight = 16.0f;
  static inline constexpr float _kSectionHeight = 8.0f;

  ax::ui::ImageView* _background;

  ax::Label* _name;
  ax::Label* _level;
  ax::Label* _health;
  ax::Label* _magicka;
  ax::Label* _stamina;

  ax::Label* _attackRange;
  ax::Label* _attackSpeed;
  ax::Label* _moveSpeed;
  ax::Label* _jumpHeight;

  ax::Label* _str;
  ax::Label* _dex;
  ax::Label* _int;
  ax::Label* _luk;
};

} // namespace vigilante

#endif // VIGILANTE_STATS_PANE_H_
