// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "ItemListView.h"

#include "AssetManager.h"
#include "Constants.h"
#include "input/Keybindable.h"
#include "item/Item.h"
#include "item/Consumable.h"
#include "ui/pause_menu/PauseMenu.h"
#include "ui/pause_menu/PauseMenuDialog.h"
#include "util/KeyCodeUtil.h"

#define VISIBLE_ITEM_COUNT 5
#define WIDTH vigilante::kVirtualWidth / 2
#define REGULAR_BG vigilante::asset_manager::kItemRegular
#define HIGHLIGHTED_BG vigilante::asset_manager::kItemHighlighted

using std::deque;
using std::vector;
using std::string;
using cocos2d::Label;

namespace vigilante {

ItemListView::ItemListView(PauseMenu* pauseMenu)
    : ListView<Item*>(pauseMenu, VISIBLE_ITEM_COUNT, WIDTH, REGULAR_BG, HIGHLIGHTED_BG) {

  // _onSelect is called at the end of ListView<T>::ListViewItem::setObject()
  // see ui/ListView.h
  this->_onSelect = [](ListViewItem* listViewItem, Item* item) {
    Label* label = listViewItem->getLabel();

    // Display item amount if amount > 1
    if (item->getAmount() > 1) {
      Label* label = listViewItem->getLabel();
      label->setString(label->getString() + " (" + std::to_string(item->getAmount()) + ")");
    }

    // Display consumable's hotkey (if defined).
    // Consumables are Keybindable, see item/Consumable.h
    Keybindable* keybindable = dynamic_cast<Keybindable*>(item);
    bool hasDefinedHotkey = keybindable && static_cast<bool>(keybindable->getHotkey());

    if (hasDefinedHotkey) {
      const string& hotkey = keycode_util::keyCodeToString(keybindable->getHotkey());
      label->setString(label->getString() + " [" + hotkey + "]");
    }
  };
}


void ItemListView::confirm() {
  Item* item = getSelectedObject();
  if (!item) {
    return;
  }

  PauseMenuDialog* dialog = _pauseMenu->getDialog();
  dialog->reset();
  dialog->setMessage("What would you like to do with " + item->getName() + "?");

  switch (item->getItemProfile().itemType) {
    case Item::Type::EQUIPMENT:
      dialog->setOption(0, true, "Equip", [=]() {
        _pauseMenu->getCharacter()->equip(dynamic_cast<Equipment*>(item));
        _pauseMenu->update();
      });
      break;
    case Item::Type::CONSUMABLE:
      dialog->setOption(0, true, "Use", [=]() {
        _pauseMenu->getCharacter()->useItem(dynamic_cast<Consumable*>(item));
        _pauseMenu->update();
      });
      break;
    case Item::Type::MISC: // fall through
    default:
      break;
  }

  dialog->setOption(1, true, "Discard", [=]() {
    _pauseMenu->getCharacter()->discardItem(item, 1);
    _pauseMenu->update();
  });
  dialog->setOption(2, true, "Cancel");
  dialog->show();
}


void ItemListView::selectUp() {
  ListView<Item*>::selectUp();

  if (!_objects[_current]) {
    _descLabel->setString("Unequip");
  }
}


void ItemListView::showItemsByType(Item::Type itemType) {
  fetchItems(itemType);

  _firstVisibleIndex = 0;
  _current = 0;
  showFrom(_firstVisibleIndex);

  // If the inventory size isn't empty, select the first item by default.
  if (_objects.size() > 0) {
    _listViewItems[0]->setSelected(true);
    _descLabel->setString(_objects[_current]->getDesc());
  } else {
    _descLabel->setString("");
  }
}

void ItemListView::showEquipmentByType(Equipment::Type equipmentType) {
  fetchEquipment(equipmentType);

  // Currently this method is only used for selecting equipment,
  // so here we're going to push_front two extra equipment.
  Character* character = _pauseMenu->getCharacter();
  Equipment* currentEquipment = character->getEquipmentSlots()[equipmentType];

  if (currentEquipment) {
    _objects.push_front(currentEquipment); // currently equipped item
    _objects.push_front(nullptr); // unequip
  }

  _firstVisibleIndex = 0;
  _current = 0;
  showFrom(_firstVisibleIndex);

  // If the inventory size isn't empty, select the first item by default.
  if (_objects.size() > 0) {
    _listViewItems[0]->setSelected(true);
    _descLabel->setString("Unequip");
  } else {
    _descLabel->setString("");
  }
}


void ItemListView::fetchItems(Item::Type itemType) {
  // Copy all items into local deque.
  const vector<Item*>& items = _pauseMenu->getCharacter()->getInventory()[itemType];
  _objects = deque<Item*>(items.begin(), items.end());
}

void ItemListView::fetchEquipment(Equipment::Type equipmentType) {
  // Copy all equipment into local deque.
  fetchItems(Item::Type::EQUIPMENT);

  // Filter out any equipment other than the specified equipmentType.
  _objects.erase(std::remove_if(_objects.begin(), _objects.end(), [=](Item* i) {
    return dynamic_cast<Equipment*>(i)->getEquipmentProfile().equipmentType != equipmentType;
  }), _objects.end());
}

} // namespace vigilante
