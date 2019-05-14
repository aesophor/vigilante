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
#include "ui/TabView.h"
#include "ItemScrollView.h"

namespace vigilante {

class InventoryPane {
 public:
  InventoryPane();
  virtual ~InventoryPane() = default;

  void selectTab(Item::Type itemType);
  cocos2d::ui::Layout* getLayout() const;

 private:
  cocos2d::ui::Layout* _layout;
  cocos2d::ui::ImageView* _background;
  cocos2d::Label* _itemDesc;
  std::unique_ptr<TabView> _tabView;
  std::unique_ptr<ItemScrollView> _itemScrollView;
};

} // namespace vigilante

#endif // VIGILANTE_INVENTORY_PANE_H_
