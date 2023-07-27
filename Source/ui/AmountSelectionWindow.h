// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_AMOUNT_SELECTION_WINDOW_H_
#define VIGILANTE_AMOUNT_SELECTION_WINDOW_H_

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

class AmountSelectionWindow : public Window {
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

#endif  // VIGILANTE_AMOUNT_SELECTION_WINDOW_H_
