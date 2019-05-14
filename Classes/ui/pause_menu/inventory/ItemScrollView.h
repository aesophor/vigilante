#ifndef VIGILANTE_ITEM_SCROLL_VIEW_H_
#define VIGILANTE_ITEM_SCROLL_VIEW_H_

#include "cocos2d.h"
#include "ui/UILayout.h"
#include "ui/UIImageView.h"
#include "ui/UIScrollView.h"

#include "item/Item.h"

namespace vigilante {

class ItemScrollView {
 public:
  ItemScrollView();
  virtual ~ItemScrollView() = default;

  void addItem(Item* item);
  cocos2d::ui::ScrollView* getScrollView() const;

 private:
  class ScrollViewItem {
   public:
    friend class ItemScrollView;
    ScrollViewItem(ItemScrollView* parent, Item* item);
    virtual ~ScrollViewItem() = default;

    void setIsSelected(bool isSelected);
    cocos2d::ui::Layout* getLayout() const;
    Item* getItem() const;

   private:
    ItemScrollView* _parent;
    cocos2d::ui::Layout* _layout;
    cocos2d::ui::ImageView* _icon;
    cocos2d::Label* _label;
    Item* _item;
  };

  cocos2d::ui::ScrollView* _scrollView;
};

} // namespace vigilante

#endif // VIGILANTE_ITEM_SCROLL_VIEW_H_
