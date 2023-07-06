// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_TRADE_LIST_VIEW_H_
#define VIGILANTE_TRADE_LIST_VIEW_H_

#include "character/Character.h"
#include "item/Item.h"
#include "ui/ListView.h"

namespace vigilante {

// Forward declaration
class TradeWindow;

class TradeListView : public ListView<Item*> {
 public:
  explicit TradeListView(TradeWindow* tradeWindow);
  virtual ~TradeListView() = default;

  virtual void confirm() override;  // ListView<Item*>
  virtual void selectUp() override;  // ListView<Item*>
  virtual void selectDown() override;  // ListView<Item*>

  void showCharactersItemByType(Character* owner, Item::Type itemType);

 private:
  void doTrade(Character* buyer,
               Character* seller,
               Item* item,
               const int amount) const;

  TradeWindow* _tradeWindow;
  ax::Label* _descLabel;
};

}  // namespace vigilante

#endif  // VIGILANTE_TRADE_LIST_VIEW_H_
