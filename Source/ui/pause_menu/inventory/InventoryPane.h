// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_UI_PAUSE_MENU_INVENTORY_INVENTORY_PANE_H_
#define VIGILANTE_UI_PAUSE_MENU_INVENTORY_INVENTORY_PANE_H_

#include <memory>
#include <string>
#include <vector>

#include <axmol.h>
#include <2d/Label.h>
#include <ui/UIImageView.h>

#include "item/Equipment.h"
#include "ui/TabView.h"
#include "ui/pause_menu/AbstractPane.h"
#include "ui/pause_menu/inventory/ItemListView.h"

namespace vigilante {

class InventoryPane final : public AbstractPane {
 public:
  InventoryPane(PauseMenu* pauseMenu);
  virtual ~InventoryPane() = default;

  virtual void update() override;
  virtual void handleInput() override;

  void selectEquipment(Equipment::Type equipmentType);

 private:
  ax::ui::ImageView* _background;
  std::unique_ptr<TabView> _tabView;
  std::unique_ptr<ItemListView> _itemListView;

  // These two variables are necessary when
  // the inventory pane is to select a equipment.
  bool _isSelectingEquipment{};
  Equipment::Type _selectingEquipmentType{};
};

}  // namespace vigilante

#endif  // VIGILANTE_UI_PAUSE_MENU_INVENTORY_INVENTORY_PANE_H_
