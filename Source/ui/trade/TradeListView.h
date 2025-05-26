// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_UI_TRADE_TRADE_LIST_VIEW_H_
#define REQUIEM_UI_TRADE_TRADE_LIST_VIEW_H_

#include "character/Character.h"
#include "item/Item.h"
#include "ui/ListView.h"

namespace requiem {

// Forward declaration
class TradeWindow;

class TradeListView final : public ListView<Item*> {
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

}  // namespace requiem

#endif  // REQUIEM_UI_TRADE_TRADE_LIST_VIEW_H_
