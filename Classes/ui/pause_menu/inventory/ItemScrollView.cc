#include "ItemScrollView.h"

#include "ui/TableLayout.h"
#include "GameAssetManager.h"

using cocos2d::Label;
using cocos2d::ui::Layout;
using cocos2d::ui::ImageView;
using cocos2d::ui::ScrollView;
using vigilante::asset_manager::kRegularFont;
using vigilante::asset_manager::kRegularFontSize;
using vigilante::asset_manager::kItemRegular;
using vigilante::asset_manager::kItemHighlighted;

namespace vigilante {

ItemScrollView::ItemScrollView() : _scrollView(ScrollView::create()) {
  _scrollView->setDirection(ScrollView::Direction::VERTICAL);
  _scrollView->setAnchorPoint({0, 1});
  _scrollView->setContentSize({300, 125});
  _scrollView->setBounceEnabled(true);
  _scrollView->setScrollBarEnabled(false);

  ImageView* bg = ImageView::create(kItemRegular);
  ImageView* icon = ImageView::create("Texture/Item/RustyAxe/icon.png");
  Label* name = Label::createWithTTF("Rusty Axe", kRegularFont, kRegularFontSize);
  name->getFontAtlas()->setAliasTexParameters();
///
/*
  Layout* lol = Layout::create();
  lol->setPosition({5, _scrollView->getContentSize().height - 5});
  bg->setAnchorPoint({0, 1});
  lol->addChild(bg);
  icon->setAnchorPoint({0, 1});
  icon->setPosition({5, -1});
  lol->addChild(icon);

  name->setAnchorPoint({0, 1});
  name->setPosition({5 + icon->getContentSize().width + 5, -2});

  lol->addChild(name);
*/

  TableLayout* layout = TableLayout::create(_scrollView->getContentSize().width);
  layout->setPosition({5, _scrollView->getContentSize().height - 5});

  bg->setAnchorPoint({0, 1});
  layout->addChild(bg);
  layout->row(1);

  layout->addChild(icon);
  layout->align(TableLayout::Direction::LEFT)->padLeft(5.0f)->spaceX(5.0f);

  name->setAnchorPoint({0, 1});
  layout->addChild(name);
  layout->padTop(1);

  _scrollView->addChild(layout);
  
  //_scrollView->addChild(lol);
}

void ItemScrollView::addItem(Item* item) {

}

ScrollView* ItemScrollView::getScrollView() const {
  return _scrollView;
}


ItemScrollView::ScrollViewItem::ScrollViewItem(ItemScrollView* parent, Item* item)
    : _parent(parent),
      _layout(Layout::create()),
      _icon(ImageView::create(item->getIconPath())),
      _label(Label::createWithTTF(item->getName(), kRegularFont, kRegularFontSize)),
      _item(item) {
  _layout->setLayoutType(Layout::Type::ABSOLUTE);
  _label->getFontAtlas()->setAliasTexParameters();
}

Layout* ItemScrollView::ScrollViewItem::getLayout() const {
  return _layout;
}

Item* ItemScrollView::ScrollViewItem::getItem() const {
  return _item;
}

} // namespace vigilante
