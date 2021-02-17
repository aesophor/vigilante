// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_OPTION_LIST_VIEW_H_
#define VIGILANTE_OPTION_LIST_VIEW_H_

#include <string>
#include <functional>

#include "ui/ListView.h"

namespace vigilante {

class PauseMenu;

using Option = std::pair<std::string, std::function<void ()>>;

class OptionListView : public ListView<Option*> {
 public:
  explicit OptionListView(PauseMenu* pauseMenu);
  virtual ~OptionListView() = default;

  virtual void confirm() override; // ListView<Option>

 private:
  PauseMenu* _pauseMenu;
};

} // namespace vigilante

#endif // VIGILANTE_OPTION_LIST_VIEW_H_
