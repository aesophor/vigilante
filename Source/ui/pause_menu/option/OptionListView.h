// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_UI_PAUSE_MENU_OPTION_OPTION_LIST_VIEW_H_
#define REQUIEM_UI_PAUSE_MENU_OPTION_OPTION_LIST_VIEW_H_

#include <functional>
#include <string>

#include "ui/ListView.h"

namespace requiem {

class PauseMenu;

using Option = std::pair<std::string, std::function<void ()>>;

class OptionListView final : public ListView<Option*> {
 public:
  explicit OptionListView(PauseMenu* pauseMenu);
  virtual ~OptionListView() = default;

  virtual void confirm() override; // ListView<Option>

 private:
  PauseMenu* _pauseMenu;
};

}  // namespace requiem

#endif  // REQUIEM_UI_PAUSE_MENU_OPTION_OPTION_LIST_VIEW_H_
