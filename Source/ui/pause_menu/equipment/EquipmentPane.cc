// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "EquipmentPane.h"

#include "Assets.h"
#include "Constants.h"
#include "character/Player.h"
#include "input/InputManager.h"
#include "ui/pause_menu/PauseMenu.h"
#include "ui/pause_menu/inventory/InventoryPane.h"

using namespace std;
using namespace vigilante::assets;
USING_NS_AX;

namespace vigilante {

EquipmentPane::EquipmentPane(PauseMenu* pauseMenu) : AbstractPane{pauseMenu, TableLayout::create(300)} {
  ui::Layout* innerLayout = ui::Layout::create();

  for (int i = 0; i < Equipment::Type::SIZE; i++) {
    EquipmentPane* parent = this;
    const string& title = Equipment::kEquipmentTypeStr[i];
    float x = 0;
    float y = -30.0f * i;
    _equipmentItems.push_back(std::make_unique<EquipmentItem>(parent, title, x, y));
    innerLayout->addChild(_equipmentItems[i]->getLayout());
  }
  _equipmentItems.front()->setSelected(true);

  _layout->addChild(innerLayout);
}

void EquipmentPane::update() {
  const Character::EquipmentSlots& slots = _pauseMenu->getPlayer()->getEquipmentSlots();

  for (int i = 0; i < Equipment::Type::SIZE; i++) {
    Equipment* equipment = slots[i];
    _equipmentItems[i]->setEquipment(equipment);
  }
}

void EquipmentPane::handleInput() {
  if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_UP_ARROW)) {
    selectUp();
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
    selectDown();
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_ENTER)) {
    confirm();
  }
}

void EquipmentPane::selectUp() {
  if (_current == 0) {
    return;
  }
  _equipmentItems[_current--]->setSelected(false);
  _equipmentItems[_current]->setSelected(true);
}

void EquipmentPane::selectDown() {
  if (_current == (int) _equipmentItems.size() - 1) {
    return;
  }
  _equipmentItems[_current++]->setSelected(false);
  _equipmentItems[_current]->setSelected(true);
}

void EquipmentPane::confirm() {
  // When user hits ENTER on a equipment slot, navigate to inventory pane
  // and switch to equipment selection mode.
  _pauseMenu->show(PauseMenu::Pane::INVENTORY);
  InventoryPane* inventoryPane = dynamic_cast<InventoryPane*>(_pauseMenu->getCurrentPane());

  inventoryPane->selectEquipment(getSelectedEquipmentType());
}

Equipment* EquipmentPane::getSelectedEquipment() const {
  return _equipmentItems[_current]->getEquipment();
}

Equipment::Type EquipmentPane::getSelectedEquipmentType() const {
  return static_cast<Equipment::Type>(_current);
}

EquipmentPane::EquipmentItem::EquipmentItem(EquipmentPane* parent, const string& title, float x, float y)
    : _parent{parent},
      _layout{TableLayout::create(300)}, // FIXME: remove this literal
      _background{ui::ImageView::create(string{kEquipmentRegular})},
      _icon{ui::ImageView::create(string{kEmptyImage})},
      _equipmentTypeLabel{Label::createWithTTF(title, string{kTitleFont}, kRegularFontSize)},
      _equipmentNameLabel{Label::createWithTTF("---", string{kBoldFont}, kRegularFontSize)} {
  _icon->setScale((float) _kEquipmentIconSize / kIconSize);

  _background->setAnchorPoint({0, 1});
  _layout->setPosition({x, y});
  _layout->addChild(_background);
  _layout->row(1);

  _layout->addChild(_icon);
  _layout->align(TableLayout::Alignment::LEFT)->padLeft(8)->spaceX(5);

  _equipmentTypeLabel->setAnchorPoint({0, 1});
  _equipmentTypeLabel->getFontAtlas()->setAliasTexParameters();
  _layout->addChild(_equipmentTypeLabel);
  _layout->padTop(1);

  _equipmentNameLabel->setAnchorPoint({0, 1});
  _equipmentNameLabel->getFontAtlas()->setAliasTexParameters();
  _layout->addChild(_equipmentNameLabel);
  _layout->align(TableLayout::Alignment::CENTER)->padTop(1);
}

void EquipmentPane::EquipmentItem::setEquipment(Equipment* equipment) {
  _equipment = equipment;

  if (equipment) {
    _icon->loadTexture(equipment->getIconPath().native());
    _equipmentNameLabel->setString(equipment->getItemProfile().name);
  } else {
    _icon->loadTexture(string{kEmptyImage});
    _equipmentNameLabel->setString("---");
  }
}

void EquipmentPane::EquipmentItem::setSelected(bool selected) const {
  _background->loadTexture(selected ? string{kEquipmentHighlighted} : string{kEquipmentRegular});
}

}  // namespace vigilante
