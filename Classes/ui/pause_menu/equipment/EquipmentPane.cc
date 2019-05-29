#include "EquipmentPane.h"

#include "AssetManager.h"
#include "Constants.h"
#include "input/GameInputManager.h"
#include "ui/pause_menu/PauseMenu.h"
#include "ui/pause_menu/inventory/InventoryPane.h"

using std::string;
using std::unique_ptr;
using cocos2d::Label;
using cocos2d::ui::Layout;
using cocos2d::ui::ImageView;
using cocos2d::EventKeyboard;
using vigilante::asset_manager::kTitleFont;
using vigilante::asset_manager::kBoldFont;
using vigilante::asset_manager::kRegularFontSize;
using vigilante::asset_manager::kEquipmentRegular;
using vigilante::asset_manager::kEquipmentHighlighted;
using vigilante::asset_manager::kEmptyItemIcon;

namespace vigilante {

EquipmentPane::EquipmentPane(PauseMenu* pauseMenu)
    : AbstractPane(pauseMenu, TableLayout::create(300)),
      _inputMgr(GameInputManager::getInstance()),
      _current() {
  Layout* innerLayout = Layout::create();

  for (int i = 0; i < Equipment::Type::SIZE; i++) {
    EquipmentPane* parent = this;
    const string& title = Equipment::_kEquipmentTypeStr[i];
    float x = 0;
    float y = -30.0f * i;
    _equipmentItems.push_back(unique_ptr<EquipmentItem>(new EquipmentItem(parent, title, x, y)));
    innerLayout->addChild(_equipmentItems[i]->getLayout());
  }
  _equipmentItems.front()->setSelected(true);

  _layout->addChild(innerLayout);
}

void EquipmentPane::update() {
  const Character::EquipmentSlots& slots = _pauseMenu->getCharacter()->getEquipmentSlots();

  for (int i = 0; i < Equipment::Type::SIZE; i++) {
    Equipment* equipment = slots[i];
    _equipmentItems[i]->setEquipment(equipment);
  }
}

void EquipmentPane::handleInput() {
  if (_inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_UP_ARROW)) {
    selectUp();
  } else if (_inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
    selectDown();
  } else if (_inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_ENTER)) {
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


const int EquipmentPane::EquipmentItem::_kEquipmentIconSize = 16;

EquipmentPane::EquipmentItem::EquipmentItem(EquipmentPane* parent, const string& title, float x, float y)
    : _parent(parent),
      _layout(TableLayout::create(300)), // FIXME: remove this literal
      _background(ImageView::create(kEquipmentRegular)),
      _icon(ImageView::create(kEmptyItemIcon)),
      _equipmentTypeLabel(Label::createWithTTF(title, kTitleFont, kRegularFontSize)),
      _equipmentNameLabel(Label::createWithTTF("---", kBoldFont, kRegularFontSize)),
      _equipment() {
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

Equipment* EquipmentPane::EquipmentItem::getEquipment() const {
  return _equipment;
}

void EquipmentPane::EquipmentItem::setEquipment(Equipment* equipment) {
  _equipment = equipment;

  if (equipment) {
    _icon->loadTexture(equipment->getIconPath());
    _equipmentNameLabel->setString(equipment->getItemProfile().name);
  } else {
    _icon->loadTexture(kEmptyItemIcon);
    _equipmentNameLabel->setString("---");
  }
}

void EquipmentPane::EquipmentItem::setSelected(bool selected) const {
  _background->loadTexture((selected) ? kEquipmentHighlighted : kEquipmentRegular);
}

Layout* EquipmentPane::EquipmentItem::getLayout() const {
  return _layout;
}

} // namespace vigilante
