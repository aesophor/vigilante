// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_UI_AMOUNT_SELECTION_WINDOW_H_
#define VIGILANTE_UI_AMOUNT_SELECTION_WINDOW_H_

#include <functional>
#include <memory>

#include <axmol.h>
#include <ui/UIImageView.h>
#include <ui/UILayout.h>

#include "ui/Window.h"
#include "ui/TextField.h"
#include "character/Character.h"
#include "ui/TabView.h"
#include "ui/trade/TradeListView.h"

namespace vigilante {

class AmountSelectionWindow final : public Window {
 public:
  AmountSelectionWindow();
  virtual ~AmountSelectionWindow();

  virtual void update(const float delta) override;  // Window
  virtual void handleInput() override;  // Window

  inline TextField* getTextField() { return &_textField; }

 private:
  ax::ui::ImageView* _contentBackground;
  TextField _textField;
};

}  // namespace vigilante

#endif  // VIGILANTE_UI_AMOUNT_SELECTION_WINDOW_H_
