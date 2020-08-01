// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_TRADE_LIST_VIEW_H_
#define VIGILANTE_TRADE_LIST_VIEW_H_

#include "item/Item.h"
#include "ui/ListView.h"
#include "ui/trade/TradeWindow.h"

namespace vigilante {

class TradeListView : public ListView<Item*> {
 public:
  explicit TradeListView(TradeWindow* tradeWindow);
  virtual ~TradeListView() = default;

  virtual void confirm() override;  // ListView<Item*>
  virtual void selectUp() override;  // ListView<Item*>
  virtual void selectDown() override;  // ListView<Item*>

 private:
  TradeWindow* _tradeWindow;
};

}  // namespace vigilante

#endif  // VIGILANTE_TRADE_LIST_VIEW_H_
