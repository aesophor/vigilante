#include "ItemListView.h"

#include "AssetManager.h"
#include "Constants.h"
#include "map/GameMapManager.h"
#include "ui/pause_menu/PauseMenu.h"
#include "ui/pause_menu/PauseMenuDialog.h"

using std::deque;
using std::vector;
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
  fetchItems(itemType);

  _firstVisibleIndex = 0;
  _current = 0;
  showItemsFrom(_firstVisibleIndex);

  // If the inventory size isn't empty, select the first item by default.
  if (_characterItems.size() > 0) {
    _listViewItems[0]->setSelected(true);
    _itemDesc->setString(_characterItems[_current]->getItemProfile().desc);
  } else {
    _itemDesc->setString("");
  }
}

void ItemListView::showEquipmentByType(Equipment::Type equipmentType) {
  fetchEquipment(equipmentType);

  // Currently this method is only used for selecting equipment,
  // so here we're going to push_front two extra equipment.
  Character* character = _pauseMenu->getCharacter();
  Equipment* currentEquipment = character->getEquipmentSlots()[equipmentType];

  if (currentEquipment) {
    _characterItems.push_front(currentEquipment); // currently equipped item
    _characterItems.push_front(nullptr); // unequip
  }

  _firstVisibleIndex = 0;
  _current = 0;
  showItemsFrom(_firstVisibleIndex);

  // If the inventory size isn't empty, select the first item by default.
  if (_characterItems.size() > 0) {
    _listViewItems[0]->setSelected(true);
    _itemDesc->setString("Unequip");
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

  if (_characterItems[_current]) {
    _itemDesc->setString(_characterItems[_current]->getItemProfile().desc);
  } else {
    _itemDesc->setString("Unequip");
  }
}

void ItemListView::selectDown() {
  if (_current >= (int) _characterItems.size() - 1) {
    return;
  }

  // If currently selected item is the last visible item, and we still can scroll down,
  // then update the selected item.
  if (_current == _firstVisibleIndex + _visibleItemCount - 1) {
    scrollDown();
  }
  _listViewItems[_current - _firstVisibleIndex]->setSelected(false);
  _listViewItems[++_current - _firstVisibleIndex]->setSelected(true);
  _itemDesc->setString(_characterItems[_current]->getItemProfile().desc);
}

void ItemListView::confirm() {
  Item* item = getSelectedItem();
  if (!item) {
    return;
  }

  PauseMenuDialog* dialog = _pauseMenu->getDialog();
  dialog->reset();
  dialog->setMessage("What would you like to do with " + item->getItemProfile().name + "?");
  dialog->setOption(0, true, "Equip", [=]() {
    _pauseMenu->getCharacter()->equip(dynamic_cast<Equipment*>(item));
    _pauseMenu->update();
  });
  dialog->setOption(1, true, "Discard", [=]() {
    _pauseMenu->getCharacter()->discardItem(item);
    _pauseMenu->update();
  });
  dialog->setOption(2, true, "Cancel");
  dialog->show();
}


void ItemListView::scrollUp() {
  if ((int) _characterItems.size() <= _visibleItemCount || _firstVisibleIndex == 0) {
    return;
  }
  showItemsFrom(--_firstVisibleIndex);
}

void ItemListView::scrollDown() {
  if ((int) _characterItems.size() <= _visibleItemCount ||
      (int) _characterItems.size() <= _firstVisibleIndex + _visibleItemCount) {
    return;
  }
  showItemsFrom(++_firstVisibleIndex);
}

void ItemListView::showItemsFrom(int index) {
  // Show n items starting from the given index.
  for (int i = 0; i < _visibleItemCount; i++) {
    _listViewItems[i]->setSelected(false);

    if (index < (int) _characterItems.size()) {
      _listViewItems[i]->setVisible(true);
      _listViewItems[i]->setItem(_characterItems[index]);
      index++;
    } else {
      _listViewItems[i]->setVisible(false);
    }
  }
}

void ItemListView::fetchItems(Item::Type itemType) {
  // Copy all items into local deque.
  const vector<Item*>& items = _pauseMenu->getCharacter()->getInventory()[itemType];
  _characterItems = deque<Item*>(items.begin(), items.end());
}

void ItemListView::fetchEquipment(Equipment::Type equipmentType) {
  // Copy all equipment into local deque.
  fetchItems(Item::Type::EQUIPMENT);

  // Filter out any equipment other than the specified equipmentType.
  _characterItems.erase(std::remove_if(_characterItems.begin(), _characterItems.end(), [=](Item* i) {
    return dynamic_cast<Equipment*>(i)->getEquipmentProfile().equipmentType != equipmentType;
  }), _characterItems.end());
}


Item* ItemListView::getSelectedItem() const {
  if (!_characterItems.empty() && _listViewItems[_current]) {
    return _listViewItems[_current]->getItem();
  }
  return nullptr;
}

Layout* ItemListView::getLayout() const {
  return _layout;
}


const int ItemListView::ListViewItem::_kListViewIconSize = 16;

ItemListView::ListViewItem::ListViewItem(ItemListView* parent, float x, float y)
    : _parent(parent),
      _layout(TableLayout::create(300)), // FIXME: remove this literal god dammit
      _background(ImageView::create(kItemRegular)),
      _icon(ImageView::create(kEmptyItemIcon)),
      _label(Label::createWithTTF("---", kRegularFont, kRegularFontSize)),
      _item() {
  _icon->setScale((float) _kListViewIconSize / kIconSize);

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
  _icon->loadTexture((item) ? item->getIconPath() : kEmptyItemIcon);
  _label->setString((item) ? item->getItemProfile().name : "---");
}

Layout* ItemListView::ListViewItem::getLayout() const {
  return _layout;
}

} // namespace vigilante
