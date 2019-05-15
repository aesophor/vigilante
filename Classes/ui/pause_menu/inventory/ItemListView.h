#ifndef VIGILANTE_ITEM_LIST_VIEW_H_
#define VIGILANTE_ITEM_LIST_VIEW_H_

#include <vector>
#include <memory>

#include "cocos2d.h"
#include "ui/UILayout.h"
#include "ui/UIImageView.h"
#include "ui/UIScrollView.h"

#include "character/Character.h"
#include "item/Item.h"
#include "ui/TableLayout.h"

namespace vigilante {

class ItemListView {
 public:
  ItemListView(Character* character, int visibleItemCount=5);
  virtual ~ItemListView() = default;

  void showItemsByType(Item::Type itemType);
  void selectUp();
  void selectDown();
  bool scrollUp();
  bool scrollDown();
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
    ItemListView* _parent;
    TableLayout* _layout;
    cocos2d::ui::ImageView* _background;
    cocos2d::ui::ImageView* _icon;
    cocos2d::Label* _label;
    Item* _item;
  };

  // Show n items starting from the given index.
  void showItemsFrom(int index); 

  Character* _character;
  cocos2d::ui::Layout* _layout;

  std::vector<std::unique_ptr<ItemListView::ListViewItem>> _listViewItems;
  const std::vector<Item*>* _characterItems;
  int _visibleItemCount;
  int _firstVisibleIndex;
  int _current;
};

} // namespace vigilante

#endif // VIGILANTE_ITEM_SCROLL_VIEW_H_
