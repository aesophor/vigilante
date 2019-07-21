// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_ITEM_LIST_VIEW_H_
#define VIGILANTE_ITEM_LIST_VIEW_H_

#include "item/Item.h"
#include "item/Equipment.h"
#include "ui/ListView.h"

namespace vigilante {

class PauseMenu;

class ItemListView : public ListView<Item*> {
 public:
  ItemListView(PauseMenu* pauseMenu);
  virtual ~ItemListView() = default;

  virtual void confirm() override; // ListView<Item*>
  virtual void selectUp() override; // ListView<Item*>
  virtual void selectDown() override; // ListView<Item*>

  void showItemsByType(Item::Type itemType);
  void showEquipmentByType(Equipment::Type equipmentType);

  void fetchItems(Item::Type itemType);
  void fetchEquipment(Equipment::Type equipmentType);

 private:
  PauseMenu* _pauseMenu;
  cocos2d::Label* _descLabel;
};

} // namespace vigilante

#endif // VIGILANTE_ITEM_LIST_VIEW_H_
