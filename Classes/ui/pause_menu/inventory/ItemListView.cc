#include "ItemListView.h"

#include "GameAssetManager.h"
#include "map/GameMapManager.h"

using std::unique_ptr;
using cocos2d::Label;
using cocos2d::ui::Layout;
using cocos2d::ui::ImageView;
using cocos2d::ui::ScrollView;
using vigilante::asset_manager::kEmptyItemIcon;
using vigilante::asset_manager::kRegularFont;
using vigilante::asset_manager::kRegularFontSize;
using vigilante::asset_manager::kItemRegular;
using vigilante::asset_manager::kItemHighlighted;

namespace vigilante {

ItemListView::ItemListView(Character* character, int visibleItemCount)
    : _character(character),
      _layout(Layout::create()),
      _visibleItemCount(visibleItemCount),
      _firstVisibleIndex(-1),
      _current(-1) {
  for (int i = 0; i < visibleItemCount; i++) {
    ItemListView* parent = this;
    float x = 5;
    float y = -5.0f - i * 25;
    _listViewItems.push_back(unique_ptr<ListViewItem>(new ListViewItem(parent, x, y)));
    _listViewItems.back()->setVisible(false);
    _layout->addChild(_listViewItems[i]->getLayout());
  }
}


Item* ItemListView::getSelectedItem() const {
  return _listViewItems[_current]->getItem();
}

Layout* ItemListView::getLayout() const {
  return _layout;
}

void ItemListView::showItemsByType(Item::Type itemType) {
  _characterItems = &(_character->getInventory()[itemType]);
  _firstVisibleIndex = 0;
  _current = 0;
  showItemsFrom(_firstVisibleIndex);
}

void ItemListView::selectUp() {
  // If currently selected item is the first visible item, and we still can scroll up,
  // then update the selected item.
  if (_current <= 0) {
    return;
  }

  if (_current == _firstVisibleIndex) {
    scrollUp();
  }
  _listViewItems[_current - _firstVisibleIndex]->setSelected(false);
  _listViewItems[--_current - _firstVisibleIndex]->setSelected(true);
}

void ItemListView::selectDown() {
  if (_current >= (int) (*_characterItems).size()) {
    return;
  }

  // If currently selected item is the last visible item, and we still can scroll down,
  // then update the selected item.
  if (_current == _firstVisibleIndex + _visibleItemCount - 1) {
    scrollDown();
    _current++;
  } else {
    _listViewItems[_current - _firstVisibleIndex]->setSelected(false);
    _current++;
    _listViewItems[_current - _firstVisibleIndex]->setSelected(true);
  }
}

void ItemListView::scrollUp() {
  if ((int) (*_characterItems).size() <= _visibleItemCount || _firstVisibleIndex == 0) {
    return;
  }
  showItemsFrom(--_firstVisibleIndex);
}

void ItemListView::scrollDown() {
  if ((int) (*_characterItems).size() <= _visibleItemCount ||
      (int) (*_characterItems).size() <= _firstVisibleIndex + _visibleItemCount) {
    return;
  }
  showItemsFrom(++_firstVisibleIndex);
}

void ItemListView::showItemsFrom(int index) {
  // Show n items starting from the given index.
  for (int i = 0; i < _visibleItemCount; i++) {
    if (index < (int) (*_characterItems).size()) {
      _listViewItems[i]->setVisible(true);
      Item* item = (*_characterItems)[index];
      _listViewItems[i]->setItem(item);
      index++;
    } else {
      _listViewItems[i]->setVisible(false);
    }
  }
}


ItemListView::ListViewItem::ListViewItem(ItemListView* parent, float x, float y)
    : _parent(parent),
      _layout(TableLayout::create(300)), // FIXME: remove this literal god dammit
      _background(ImageView::create(kItemRegular)),
      _icon(ImageView::create(kEmptyItemIcon)),
      _label(Label::createWithTTF("---", kRegularFont, kRegularFontSize)),
      _item() {
  _background->setAnchorPoint({0, 1});
  _layout->setPosition({x, y});
  _layout->addChild(_background);
  _layout->row(1);

  _layout->addChild(_icon);
  _layout->align(TableLayout::Direction::LEFT)->padLeft(5)->spaceX(5);

  _label->setAnchorPoint({0, 1});
  _label->getFontAtlas()->setAliasTexParameters();
  _layout->addChild(_label);
  _layout->padTop(1);
}

void ItemListView::ListViewItem::setSelected(bool selected) {
  _background->loadTexture((selected) ? kItemHighlighted : kItemRegular);
}

void ItemListView::ListViewItem::setVisible(bool visible) {
  _layout->setVisible(visible);
}

Layout* ItemListView::ListViewItem::getLayout() const {
  return _layout;
}

Item* ItemListView::ListViewItem::getItem() const {
  return _item;
}

void ItemListView::ListViewItem::setItem(Item* item) {
  _item = item;
  _icon->loadTexture(item->getIconPath());
  _label->setString(item->getName());
}

} // namespace vigilante
