// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_OPTION_PANE_H_
#define VIGILANTE_OPTION_PANE_H_

#include <vector>
#include <memory>

#include "ui/pause_menu/AbstractPane.h"
#include "ui/pause_menu/option/OptionListView.h"

#define OPTION_COUNT 4

namespace vigilante {

class OptionPane : public AbstractPane {
 public:
  explicit OptionPane(PauseMenu* pauseMenu);
  virtual ~OptionPane() = default;

  virtual void update() override;
  virtual void handleInput() override;

 private:
  std::vector<Option> _options;
  std::unique_ptr<OptionListView> _optionListView;
};

} // namespace vigilante

#endif // VIGILANTE_OPTION_PANE_H_
