// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "InventoryPane.h"

#include "std/make_unique.h"
#include "AssetManager.h"
#include "Constants.h"
#include "character/Player.h"
#include "input/InputManager.h"
#include "map/GameMapManager.h"
#include "ui/pause_menu/PauseMenu.h"

using std::string;
using cocos2d::Label;
using cocos2d::ui::Layout;
using cocos2d::ui::ImageView;
using cocos2d::EventKeyboard;
using vigilante::asset_manager::kInventoryBg;
using vigilante::asset_manager::kTabRegular;
using vigilante::asset_manager::kTabHighlighted;

namespace vigilante {

InventoryPane::InventoryPane(PauseMenu* pauseMenu)
    : AbstractPane(pauseMenu),
      _isSelectingEquipment(),
      _selectingEquipmentType(),
      _background(ImageView::create(kInventoryBg)),
      _tabView(std::make_unique<TabView>(kTabRegular, kTabHighlighted)),
      _itemListView(std::make_unique<ItemListView>(pauseMenu)) {
  _background->setAnchorPoint({0, 1});

  _layout->setLayoutType(Layout::Type::ABSOLUTE);
  _layout->setAnchorPoint({0, 1}); // Make top-left (0, 0)
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
  auto inputMgr = InputManager::getInstance();

  if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_LEFT_ARROW)) {
    _tabView->selectPrev();
    update();
  } else if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_RIGHT_ARROW)) {
    _tabView->selectNext();
    update();
  } else if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_UP_ARROW)) {
    _itemListView->selectUp();
  } else if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
    _itemListView->selectDown();
  } else if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_ENTER)) {
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
