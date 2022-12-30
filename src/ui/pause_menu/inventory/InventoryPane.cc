// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "InventoryPane.h"

#include "Assets.h"
#include "Constants.h"
#include "character/Player.h"
#include "input/InputManager.h"
#include "map/GameMapManager.h"
#include "ui/control_hints/ControlHints.h"
#include "ui/pause_menu/PauseMenu.h"

using namespace std;
using namespace vigilante::assets;
USING_NS_CC;

namespace vigilante {

InventoryPane::InventoryPane(PauseMenu* pauseMenu)
    : AbstractPane(pauseMenu),
      _background(ui::ImageView::create(kInventoryBg)),
      _tabView(make_unique<TabView>(kTabRegular, kTabHighlighted)),
      _itemListView(make_unique<ItemListView>(pauseMenu)),
      _isSelectingEquipment(),
      _selectingEquipmentType() {
  _background->setAnchorPoint({0, 1});

  _layout->setLayoutType(ui::Layout::Type::ABSOLUTE);
  _layout->setAnchorPoint({0, 1});  // Make top-left (0, 0)
  _layout->addChild(_background);

  // Place item category tabs.
  _tabView->addTab("EQUIP");
  _tabView->addTab("USE");
  _tabView->addTab("MISC");
  _tabView->selectTab(0);
  _layout->addChild(_tabView->getLayout());

  // Place item list view.
  _itemListView->getLayout()->setPosition({5, -5});
  _layout->addChild(_itemListView->getLayout());
}

void InventoryPane::update() {
  Item::Type selectedItemType = static_cast<Item::Type>(_tabView->getSelectedTab()->getIndex());
  _itemListView->showItemsByType(selectedItemType);
}

void InventoryPane::handleInput() {
  if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_LEFT_ARROW)) {
    _tabView->selectPrev();
    update();
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_RIGHT_ARROW)) {
    _tabView->selectNext();
    update();
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_UP_ARROW)) {
    _itemListView->selectUp();
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
    _itemListView->selectDown();
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_ENTER)) {
    if (!_isSelectingEquipment) {
      _itemListView->confirm();
    } else {
      // If currently the inventory pane is in equipment selection mode,
      // get the selected item, and set it as the new equipment.
      Equipment* equipment = dynamic_cast<Equipment*>(_itemListView->getSelectedObject());
      if (equipment) {
        _pauseMenu->getPlayer()->equip(equipment);
      } else {
        _pauseMenu->getPlayer()->unequip(_selectingEquipmentType);
      }
      _isSelectingEquipment = false;

      // Go back to equipment pane.
      _pauseMenu->show(PauseMenu::Pane::EQUIPMENT);
    }
  }
}

void InventoryPane::selectEquipment(Equipment::Type equipmentType) {
  // Switch into equipment selection mode.
  _isSelectingEquipment = true;
  _selectingEquipmentType = equipmentType;
  _pauseMenu->show(PauseMenu::Pane::INVENTORY);

  // Show only the corresponding equipment type.
  _itemListView->showEquipmentByType(equipmentType);
}

}  // namespace vigilante
