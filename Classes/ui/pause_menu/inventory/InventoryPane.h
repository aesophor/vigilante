#ifndef VIGILANTE_INVENTORY_PANE_H_
#define VIGILANTE_INVENTORY_PANE_H_

#include <vector>
#include <string>
#include <memory>

#include "cocos2d.h"
#include "2d/CCLabel.h"
#include "ui/UILayout.h"
#include "ui/UIImageView.h"

#include "ui/pause_menu/AbstractPane.h"
#include "ui/TabView.h"
#include "item/Equipment.h"
#include "ItemListView.h"

namespace vigilante {

class InventoryPane : public AbstractPane {
 public:
  InventoryPane(PauseMenu* pauseMenu);
  virtual ~InventoryPane() = default;

  virtual void update() override;
  virtual void handleInput() override;

  void selectEquipment(Equipment::Type equipmentType);

 private:
  // These two variables are necessary when
  // the inventory pane is to select a equipment.
  bool _isSelectingEquipment;
  Equipment::Type _selectingEquipmentType;

  cocos2d::ui::ImageView* _background;
  cocos2d::Label* _itemDesc;
  std::unique_ptr<TabView> _tabView;
  std::unique_ptr<ItemListView> _itemListView;
};

} // namespace vigilante

#endif // VIGILANTE_INVENTORY_PANE_H_
