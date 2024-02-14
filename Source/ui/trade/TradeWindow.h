// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_UI_TRADE_TRADE_WINDOW_H_
#define VIGILANTE_UI_TRADE_TRADE_WINDOW_H_

#include <memory>

#include <axmol.h>
#include <ui/UIImageView.h>
#include <ui/UILayout.h>
#include "ui/Window.h"
#include "character/Character.h"
#include "ui/TabView.h"
#include "ui/trade/TradeListView.h"

namespace vigilante {

class TradeWindow : public Window {
 public:
  TradeWindow(Character* buyer, Character* seller);
  virtual ~TradeWindow() = default;

  virtual void update(const float delta) override;  // Window
  virtual void handleInput() override;  // Window

  void toggleBuySell();

  inline bool isTradingWithAlly() const { return _isTradingWithAlly; }
  inline Character* getBuyer() const { return _buyer; }
  inline Character* getSeller() const { return _seller; }

 private:
  ax::ui::ImageView* _contentBackground;
  std::unique_ptr<TabView> _tabView;
  std::unique_ptr<TradeListView> _tradeListView;

  bool _isTradingWithAlly;
  Character* _buyer;
  Character* _seller;
};

}  // namespace vigilante

#endif  // VIGILANTE_UI_TRADE_TRADE_WINDOW_H_
