// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "TradeListView.h"

#include <memory>

#include "AssetManager.h"
#include "ui/trade/TradeWindow.h"

#define VISIBLE_ITEM_COUNT 5
#define WIDTH 289.5
#define HEIGHT 120
#define ITEM_GAP_HEIGHT 25
#define REGULAR_BG vigilante::asset_manager::kItemRegular
#define HIGHLIGHTED_BG vigilante::asset_manager::kItemHighlighted

#define DESC_LABEL_X 5
#define DESC_LABEL_Y -132

#define EMPTY_ITEM_ICON vigilante::asset_manager::kEmptyImage
#define EMPTY_ITEM_NAME "---"

using cocos2d::Label;
using cocos2d::ui::ImageView;

namespace vigilante {

TradeListView::TradeListView(TradeWindow* tradeWindow)
    : ListView<Item*>(VISIBLE_ITEM_COUNT, WIDTH, HEIGHT, ITEM_GAP_HEIGHT, REGULAR_BG, HIGHLIGHTED_BG),
      _tradeWindow(tradeWindow),
      _descLabel(Label::createWithTTF("", asset_manager::kRegularFont, asset_manager::kRegularFontSize)) {

  // _setObjectCallback is called at the end of ListView<T>::ListViewItem::setObject()
  // see ui/ListView.h
  _setObjectCallback = [](ListViewItem* listViewItem, Item* item) {
    ImageView* icon = listViewItem->getIcon();
    Label* label = listViewItem->getLabel();

    icon->loadTexture((item) ? item->getIconPath() : EMPTY_ITEM_ICON);
    label->setString((item) ? item->getName() : EMPTY_ITEM_NAME);

    if (!item) {
      return;
    }

    // Display item amount if amount > 1
    if (item->getAmount() > 1) {
      Label* label = listViewItem->getLabel();
      label->setString(label->getString() + " (" + std::to_string(item->getAmount()) + ")");
    }
  };

  _descLabel->getFontAtlas()->setAliasTexParameters();
  _descLabel->setAnchorPoint({0, 1});
  _descLabel->setPosition({DESC_LABEL_X, DESC_LABEL_Y});
  _descLabel->enableWrap(true);
  _layout->addChild(_descLabel);
}


void TradeListView::confirm() {
  Item* item = getSelectedObject();
  if (!item) {
    return;
  }

  _tradeWindow->getBuyer()->addItem(Item::create(item->getItemProfile().jsonFileName));
  _tradeWindow->getSeller()->removeItem(item);

  _tradeWindow->update();
}

void TradeListView::selectUp() {
  ListView<Item*>::selectUp();

  if (_objects.empty()) {
    return;
  }

  Item* selectedItem = getSelectedObject();
  assert(selectedItem != nullptr);
  _descLabel->setString(selectedItem->getDesc());
}

void TradeListView::selectDown() {
  ListView<Item*>::selectDown();

  if (_objects.empty()) {
    return;
  }

  Item* selectedItem = getSelectedObject();
  assert(selectedItem != nullptr);
  _descLabel->setString(selectedItem->getDesc());
}


void TradeListView::showCharactersItemByType(Character* owner, Item::Type itemType) {
  // Show the owner's items of the specified type.
  setObjects(owner->getInventory()[itemType]);

  // Update description label.
  _descLabel->setString((_objects.size() > 0) ? _objects[_current]->getDesc() : "");
}

}  // namespace vigilante
