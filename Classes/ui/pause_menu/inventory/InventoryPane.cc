#include "InventoryPane.h"

#include "GameAssetManager.h"
#include "input/GameInputManager.h"
#include "map/GameMapManager.h"

using std::string;
using cocos2d::Label;
using cocos2d::ui::Layout;
using cocos2d::ui::ImageView;
using cocos2d::EventKeyboard;
using vigilante::asset_manager::kBoldFont;
using vigilante::asset_manager::kRegularFont;
using vigilante::asset_manager::kRegularFontSize;
using vigilante::asset_manager::kInventoryBg;
using vigilante::asset_manager::kTabRegular;
using vigilante::asset_manager::kTabHighlighted;

namespace vigilante {

InventoryPane::InventoryPane(PauseMenu* pauseMenu)
    : AbstractPane(),
      _pauseMenu(pauseMenu),
      _isSelectingEquipment(),
      _selectingEquipmentType(),
      _background(ImageView::create(kInventoryBg)),
      _itemDesc(Label::createWithTTF("", kRegularFont, kRegularFontSize)),
      _tabView(new TabView(kTabRegular, kTabHighlighted)),
      _itemListView(new ItemListView(pauseMenu, _itemDesc)) {
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
  _layout->addChild(_itemListView->getLayout());

  // Place item description label.
  _itemDesc->getFontAtlas()->setAliasTexParameters();
  _itemDesc->setAnchorPoint({0, 1});
  _itemDesc->setPosition({10, -137});
  _layout->addChild(_itemDesc);
}


void InventoryPane::update() {
  Item::Type selectedItemType = static_cast<Item::Type>(_tabView->getSelectedTab()->getIndex());
  _itemListView->showItemsByType(selectedItemType);
}

void InventoryPane::handleInput() {
  auto inputMgr = GameInputManager::getInstance();

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
      Equipment* equipment = dynamic_cast<Equipment*>(_itemListView->getSelectedItem());
      if (equipment) {
        _pauseMenu->getCharacter()->equip(equipment);
      } else {
        _pauseMenu->getCharacter()->unequip(_selectingEquipmentType);
      }
      _isSelectingEquipment = false;

      // Go back to equipment pane.
      _pauseMenu->show(PauseMenu::Pane::EQUIPMENT);
    }
  }
}


void InventoryPane::selectEquipment(Equipment::Type equipmentType) {
  _isSelectingEquipment = true;
  _selectingEquipmentType = equipmentType;

  // Switch into equipment selection mode.
  _pauseMenu->show(PauseMenu::Pane::INVENTORY);

  // Show only the corresponding equipment type.
  _itemListView->showEquipmentByType(equipmentType);
  cocos2d::log("show equipment by type ok");
}

} // namespace vigilante
