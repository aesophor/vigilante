// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_UI_PAUSE_MENU_OPTION_OPTION_PANE_H_
#define VIGILANTE_UI_PAUSE_MENU_OPTION_OPTION_PANE_H_

#include <memory>
#include <vector>

#include "ui/pause_menu/AbstractPane.h"
#include "ui/pause_menu/option/OptionListView.h"

namespace vigilante {

class OptionPane final : public AbstractPane {
 public:
  explicit OptionPane(PauseMenu* pauseMenu);
  virtual ~OptionPane() = default;

  virtual void update() override;
  virtual void handleInput() override;

 private:
  std::vector<Option> _options;
  std::unique_ptr<OptionListView> _optionListView;
};

}  // namespace vigilante

#endif  // VIGILANTE_UI_PAUSE_MENU_OPTION_OPTION_PANE_H_
