#ifndef VIGILANTE_INVENTORY_PANE_H_
#define VIGILANTE_INVENTORY_PANE_H_

#include <vector>
#include <string>
#include <memory>

#include "cocos2d.h"
#include "2d/CCLabel.h"
#include "ui/UILayout.h"
#include "ui/UIImageView.h"

#include "ItemListView.h"
#include "ui/pause_menu/AbstractPane.h"
#include "ui/pause_menu/PauseMenu.h"
#include "ui/TabView.h"
#include "item/Item.h"

namespace vigilante {

class PauseMenu;

class InventoryPane : public AbstractPane {
 public:
  InventoryPane(PauseMenu* parent);
  virtual ~InventoryPane() = default;

  virtual void update() override;
  virtual void handleInput() override;

 private:
  PauseMenu* _parent;

  cocos2d::ui::ImageView* _background;
  cocos2d::Label* _itemDesc;
  std::unique_ptr<TabView> _tabView;
  std::unique_ptr<ItemListView> _itemListView;
};

} // namespace vigilante

#endif // VIGILANTE_INVENTORY_PANE_H_
