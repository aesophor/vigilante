#ifndef VIGILANTE_INVENTORY_PANE_H_
#define VIGILANTE_INVENTORY_PANE_H_

#include <vector>
#include <string>
#include <memory>

#include "cocos2d.h"
#include "2d/CCLabel.h"
#include "ui/UILayout.h"
#include "ui/UIImageView.h"

#include "item/Item.h"
#include "ui/pause_menu/AbstractPane.h"
#include "ui/TabView.h"
#include "ItemListView.h"

namespace vigilante {

class InventoryPane : public AbstractPane {
 public:
  InventoryPane();
  virtual ~InventoryPane() = default;
  virtual void handleInput() override;

 private:
  cocos2d::ui::ImageView* _background;
  cocos2d::Label* _itemDesc;
  std::unique_ptr<TabView> _tabView;
  std::unique_ptr<ItemListView> _itemListView;
};

} // namespace vigilante

#endif // VIGILANTE_INVENTORY_PANE_H_
