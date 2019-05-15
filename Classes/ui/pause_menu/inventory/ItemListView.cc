#include "ItemListView.h"

#include "GameAssetManager.h"
#include "map/GameMapManager.h"
#include "ui/pause_menu/PauseMenuDialog.h"

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

ItemListView::ItemListView(PauseMenu* pauseMenu, Label* itemDesc, int visibleItemCount)
    : _pauseMenu(pauseMenu),
      _layout(Layout::create()),
      _itemDesc(itemDesc),
      _visibleItemCount(visibleItemCount),
      _firstVisibleIndex(),
      _current() {
  for (int i = 0; i < visibleItemCount; i++) {
    ItemListView* parent = this;
    float x = 5;
    float y = -5.0f - i * 25;
    _listViewItems.push_back(unique_ptr<ListViewItem>(new ListViewItem(parent, x, y)));
    _listViewItems.back()->setVisible(false);
    _layout->addChild(_listViewItems[i]->getLayout());
  }
}


void ItemListView::showItemsByType(Item::Type itemType) {
  _characterItems = &(_pauseMenu->getCharacter()->getInventory()[itemType]);
  _firstVisibleIndex = 0;
  _current = 0;
  showItemsFrom(_firstVisibleIndex);

  if ((*_characterItems).size() > 0) {
    _listViewItems[0]->setSelected(true);
    _itemDesc->setString((*_characterItems)[_current]->getDesc());
  } else {
    _itemDesc->setString("");
  }
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
  _itemDesc->setString((*_characterItems)[_current]->getDesc());
}

void ItemListView::selectDown() {
  if (_current >= (int) (*_characterItems).size() - 1) {
    return;
  }

  // If currently selected item is the last visible item, and we still can scroll down,
  // then update the selected item.
  if (_current == _firstVisibleIndex + _visibleItemCount - 1) {
    scrollDown();
  }
  _listViewItems[_current - _firstVisibleIndex]->setSelected(false);
  _listViewItems[++_current - _firstVisibleIndex]->setSelected(true);
  _itemDesc->setString((*_characterItems)[_current]->getDesc());
}

void ItemListView::confirm() {
  Item* item = getSelectedItem();
  if (!item) {
    return;
  }

  PauseMenuDialog* dialog = _pauseMenu->getDialog();
  dialog->reset();
  dialog->setMessage("What would you like to do with " + item->getName() + "?");
  dialog->setOption(0, true, "Equip", [=]() {
    _pauseMenu->getCharacter()->equip(dynamic_cast<Equipment*>(item));
    _pauseMenu->update();
  });
  dialog->setOption(1, true, "Discard");
  dialog->show();
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
    _listViewItems[i]->setSelected(false);
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


Item* ItemListView::getSelectedItem() const {
  if (_listViewItems[_current]) {
    return _listViewItems[_current]->getItem();
  }
  return nullptr;
}

Layout* ItemListView::getLayout() const {
  return _layout;
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
  _layout->align(TableLayout::Alignment::LEFT)->padLeft(5)->spaceX(5);

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

Item* ItemListView::ListViewItem::getItem() const {
  return _item;
}

void ItemListView::ListViewItem::setItem(Item* item) {
  _item = item;
  _icon->loadTexture(item->getIconPath());
  _label->setString(item->getName());
}

Layout* ItemListView::ListViewItem::getLayout() const {
  return _layout;
}

} // namespace vigilante
