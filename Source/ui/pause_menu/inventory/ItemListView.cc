// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "ItemListView.h"

#include "Assets.h"
#include "Constants.h"
#include "character/Player.h"
#include "input/Keybindable.h"
#include "item/Item.h"
#include "item/Consumable.h"
#include "ui/pause_menu/PauseMenu.h"
#include "ui/pause_menu/PauseMenuDialog.h"
#include "util/ds/SetVector.h"
#include "util/KeyCodeUtil.h"

using namespace std;
using namespace vigilante::assets;
USING_NS_AX;

namespace vigilante {

namespace {

constexpr int kVisibleItemCount = 5;
constexpr float kWidth = 289.5f;
constexpr float kHeight = 120.0f;
constexpr float kItemGapHeight = 25.0f;

constexpr float kDescLabelX = 5.0f;
constexpr float kDescLabelY = -132.0f;

constexpr char kEmptyItemName[] = "---";

}  // namespace

ItemListView::ItemListView(PauseMenu* pauseMenu)
    : ListView<Item*>{kVisibleItemCount, kWidth, kHeight, kItemGapHeight, kItemRegular, kItemHighlighted},
      _pauseMenu{pauseMenu},
      _descLabel{Label::createWithTTF("", string{kRegularFont}, kRegularFontSize)} {
  // _setObjectCallback is called at the end of ListView<T>::ListViewItem::setObject()
  // see ui/ListView.h
  _setObjectCallback = [](ListViewItem* listViewItem, Item* item) {
    ui::ImageView* icon = listViewItem->getIcon();
    Label* label = listViewItem->getLabel();

    icon->loadTexture(item ? item->getIconPath().native() : kEmptyImage.native());
    label->setString(item ? item->getName() : kEmptyItemName);

    if (!item) {
      return;
    }

    // Display item amount if amount > 1
    if (item->getAmount() > 1) {
      Label* label = listViewItem->getLabel();
      label->setString(string{label->getString()} + " (" + std::to_string(item->getAmount()) + ")");
    }

    // Display consumable's hotkey (if defined).
    // Consumables are Keybindable, see item/Consumable.h
    Keybindable* keybindable = dynamic_cast<Keybindable*>(item);
    bool hasDefinedHotkey = keybindable && static_cast<bool>(keybindable->getHotkey());

    if (hasDefinedHotkey) {
      const string& hotkey = keycode_util::keyCodeToString(keybindable->getHotkey());
      label->setString(string{label->getString()} + " [" + hotkey + "]");
    }
  };

  _descLabel->getFontAtlas()->setAliasTexParameters();
  _descLabel->setAnchorPoint({0, 1});
  _descLabel->setPosition({kDescLabelX, kDescLabelY});
  _descLabel->enableWrap(true);
  _layout->addChild(_descLabel);
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
      dialog->setOption(0, true, "Equip", [this, item]() {
        _pauseMenu->getPlayer()->equip(dynamic_cast<Equipment*>(item));
        _pauseMenu->update();
      });
      break;
    case Item::Type::CONSUMABLE:
      dialog->setOption(0, true, "Use", [this, item]() {
        _pauseMenu->getPlayer()->useItem(dynamic_cast<Consumable*>(item));
        _pauseMenu->update();
      });
      break;
    case Item::Type::MISC:  // fall through
    default:
      break;
  }

  dialog->setOption(1, true, "Discard", [this, item]() {
    _pauseMenu->getPlayer()->discardItem(item, 1);
    _pauseMenu->update();
  });
  dialog->setOption(2, true, "Cancel");
  dialog->show();
}

void ItemListView::selectUp() {
  ListView<Item*>::selectUp();

  if (_objects.empty()) {
    return;
  }

  Item* selectedItem = getSelectedObject();
  _descLabel->setString((selectedItem) ? selectedItem->getDesc() : "Unequip");
}

void ItemListView::selectDown() {
  ListView<Item*>::selectDown();

  if (_objects.empty()) {
    return;
  }

  Item* selectedItem = getSelectedObject();
  _descLabel->setString((selectedItem) ? selectedItem->getDesc() : "Unequip");
}

void ItemListView::showItemsByType(Item::Type itemType) {
  // Show items of the specified type in ItemListView.
  setObjects(_pauseMenu->getPlayer()->getInventory()[itemType]);

  // Update description label.
  _descLabel->setString(_objects.size() > 0 ? _objects[_current]->getDesc() : "");
}

void ItemListView::showEquipmentByType(Equipment::Type equipmentType) {
  const SetVector<Item*>& equipments = _pauseMenu->getPlayer()->getInventory()[Item::Type::EQUIPMENT];
  deque<Item*> objects(equipments.begin(), equipments.end());

  // Filter out any equipment other than the specified equipmentType.
  objects.erase(std::remove_if(objects.begin(), objects.end(), [=](Item* i) {
    return dynamic_cast<Equipment*>(i)->getEquipmentProfile().equipmentType != equipmentType;
  }), objects.end());

  // Currently this method is only used for selecting equipment,
  // so here we're going to push_front two extra equipment.
  Character* character = _pauseMenu->getPlayer();
  Equipment* currentEquipment = character->getEquipmentSlots()[equipmentType];

  if (currentEquipment) {
    objects.push_front(currentEquipment);
  }
  objects.push_front(nullptr);  // unequip

  // Show equipments of the specified type in ItemListView.
  setObjects(objects);

  // Update description label. The first item is an empty item,
  // Selecting it will unequip current equipment.
  _descLabel->setString(_objects.size() > 0 ? "Unequip" : "");
}

}  // namespace vigilante
