#ifndef VIGILANTE_ITEM_LIST_VIEW_H_
#define VIGILANTE_ITEM_LIST_VIEW_H_

#include <deque>
#include <vector>
#include <memory>

#include "cocos2d.h"
#include "2d/CCLabel.h"
#include "ui/UILayout.h"
#include "ui/UIImageView.h"
#include "ui/UIScrollView.h"

#include "ui/TableLayout.h"
#include "character/Character.h"
#include "item/Item.h"
#include "item/Equipment.h"

namespace vigilante {

class PauseMenu;

class ItemListView {
 public:
  ItemListView(PauseMenu* pauseMenu, cocos2d::Label* itemDesc, int visibleItemCount=5);
  virtual ~ItemListView() = default;

  void showItemsByType(Item::Type itemType);
  void showEquipmentByType(Equipment::Type equipmentType);

  void selectUp();
  void selectDown();
  void confirm();

  void scrollUp();
  void scrollDown();

  Item* getSelectedItem() const;
  cocos2d::ui::Layout* getLayout() const;

 private:
  class ListViewItem {
   public:
    friend class ItemListView;
    ListViewItem(ItemListView* parent, float x, float y);
    virtual ~ListViewItem() = default;

    void setSelected(bool selected);
    void setVisible(bool visible);

    Item* getItem() const;
    void setItem(Item* item);

    cocos2d::ui::Layout* getLayout() const;

   private:
    static const int _kListViewIconSize;

    ItemListView* _parent;
    TableLayout* _layout;
    cocos2d::ui::ImageView* _background;
    cocos2d::ui::ImageView* _icon;
    cocos2d::Label* _label;
    Item* _item;
  };

  // Show n items starting from the given index.
  void showItemsFrom(int index);
  void fetchItems(Item::Type itemType);
  void fetchEquipment(Equipment::Type equipmentType);

  PauseMenu* _pauseMenu;

  cocos2d::ui::Layout* _layout;
  cocos2d::Label* _itemDesc;

  std::vector<std::unique_ptr<ItemListView::ListViewItem>> _listViewItems;
  std::deque<Item*> _characterItems;
  int _visibleItemCount;
  int _firstVisibleIndex;
  int _current;
};

} // namespace vigilante

#endif // VIGILANTE_ITEM_SCROLL_VIEW_H_
