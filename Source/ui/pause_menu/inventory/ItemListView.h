// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_UI_PAUSE_MENU_INVENTORY_ITEM_LIST_VIEW_H_
#define REQUIEM_UI_PAUSE_MENU_INVENTORY_ITEM_LIST_VIEW_H_

#include "item/Item.h"
#include "item/Equipment.h"
#include "ui/ListView.h"

namespace requiem {

class PauseMenu;

class ItemListView final : public ListView<Item*> {
 public:
  explicit ItemListView(PauseMenu* pauseMenu);
  virtual ~ItemListView() = default;

  virtual void confirm() override;  // ListView<Item*>
  virtual void selectUp() override;  // ListView<Item*>
  virtual void selectDown() override;  // ListView<Item*>

  void showItemsByType(Item::Type itemType);
  void showEquipmentByType(Equipment::Type equipmentType);

 private:
  PauseMenu* _pauseMenu;
  ax::Label* _descLabel;
};

}  // namespace requiem

#endif  // REQUIEM_UI_PAUSE_MENU_INVENTORY_ITEM_LIST_VIEW_H_
