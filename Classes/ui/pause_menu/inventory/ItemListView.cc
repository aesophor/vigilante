#include "ItemListView.h"

#include "ui/UILayout.h"

#include "AssetManager.h"
#include "Constants.h"
#include "character/Character.h"
#include "item/Consumable.h"
#include "map/GameMapManager.h"
#include "ui/pause_menu/PauseMenu.h"
#include "ui/pause_menu/PauseMenuDialog.h"

using std::pair;
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

ItemListView::ItemListView(PauseMenu* pauseMenu, int visibleItemCount)
    : ListView<Item*>(pauseMenu, visibleItemCount) {}


void ItemListView::confirm() {
  Item* item = getSelectedObject();
  if (!item) {
    return;
  }

  PauseMenuDialog* dialog = _pauseMenu->getDialog();
  dialog->reset();
  dialog->setMessage("What would you like to do with " + item->getItemProfile().name + "?");

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
    _itemDesc->setString("Unequip");
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
    _itemDesc->setString(_objects[_current]->getItemProfile().desc);
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
    _objects.push_front(currentEquipment); // currently equipped item
    _objects.push_front(nullptr); // unequip
  }

  _firstVisibleIndex = 0;
  _current = 0;
  showFrom(_firstVisibleIndex);

  // If the inventory size isn't empty, select the first item by default.
  if (_objects.size() > 0) {
    _listViewItems[0]->setSelected(true);
    _itemDesc->setString("Unequip");
  } else {
    _itemDesc->setString("");
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
