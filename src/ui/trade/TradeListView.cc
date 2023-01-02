// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "TradeListView.h"

#include <memory>

#include "Assets.h"
#include "input/InputManager.h"
#include "gameplay/ItemPriceTable.h"
#include "ui/AmountSelectionWindow.h"
#include "ui/WindowManager.h"
#include "ui/notifications/Notifications.h"
#include "ui/trade/TradeWindow.h"
#include "util/StringUtil.h"

#define VISIBLE_ITEM_COUNT 5
#define WIDTH 289.5
#define HEIGHT 120
#define ITEM_GAP_HEIGHT 25

#define DESC_LABEL_X 5
#define DESC_LABEL_Y -132

#define EMPTY_ITEM_NAME "---"

using namespace std;
using namespace vigilante::assets;
USING_NS_CC;

namespace vigilante {

TradeListView::TradeListView(TradeWindow* tradeWindow)
    : ListView<Item*>(VISIBLE_ITEM_COUNT, WIDTH, HEIGHT, ITEM_GAP_HEIGHT, kItemRegular, kItemHighlighted),
      _tradeWindow(tradeWindow),
      _descLabel(Label::createWithTTF("", assets::kRegularFont, assets::kRegularFontSize)) {
  // _setObjectCallback is called at the end of ListView<T>::ListViewItem::setObject()
  // see ui/ListView.h
  _setObjectCallback = [this](ListViewItem* listViewItem, Item* item) {
    assert(item != nullptr);

    ui::ImageView* icon = listViewItem->getIcon();
    Label* label = listViewItem->getLabel();

    icon->loadTexture((item) ? item->getIconPath() : kEmptyImage.native());
    label->setString((item) ? item->getName() : EMPTY_ITEM_NAME);

    // Display item price if not trading with ally.
    if (!_tradeWindow->isTradingWithAlly()) {
      label->setString(label->getString() +
          string_util::format(" [$%d]", item_price_table::getPrice(item)));
    }

    // Display item amount if amount > 1
    if (item->getAmount() > 1) {
      label->setString(label->getString() +
          string_util::format(" (%d)", item->getAmount()));
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

  Character* seller = _tradeWindow->getSeller();
  Character* buyer = _tradeWindow->getBuyer();

  if (item->getAmount() == 1) {
    doTrade(buyer, seller, item, 1);

  } else {
    auto w = std::make_unique<AmountSelectionWindow>();
    AmountSelectionWindow* wRaw = w.get();

    auto onSubmit = [wRaw, this, buyer, seller, item]() {
      const string& buf = wRaw->getTextField()->getString();
      int amount = 0;

      try {
        amount = std::stoi(buf);
      } catch (const std::invalid_argument& ex) {
        Notifications::getInstance()->show("Invalid amount");
      } catch (const std::out_of_range& ex) {
        Notifications::getInstance()->show("Amount too large or too small");
      } catch (...) {
        Notifications::getInstance()->show("Unknown error while parsing amount");
      }

      doTrade(buyer, seller, item, amount);
    };

    auto onDismiss = []() {
      // Close AmountSelectionWindow which
      // should be at the top at this moment.
      WindowManager::getInstance()->pop();
    };

    w->getTextField()->setOnSubmit(onSubmit);
    w->getTextField()->setOnDismiss(onDismiss);
    w->getTextField()->setReceivingInput(true);
    WindowManager::getInstance()->push(std::move(w));
  }
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

void TradeListView::doTrade(Character* buyer,
                            Character* seller,
                            Item* item,
                            const int amount) const {
  if (amount <= 0) {
    return;
  }

  const int price = item_price_table::getPrice(item);

  // Check if `amount` is valid.
  if (amount > item->getAmount()) {
    Notifications::getInstance()->show("Invalid amount");
    return;
  }

  if (!_tradeWindow->isTradingWithAlly()) {
    // Check if the buyer has sufficient amount of gold.
    if (buyer->getGoldBalance() < price * amount) {
      Notifications::getInstance()->show("The buyer doesn't have sufficient amount of gold.");
      return;
    }

    // Transfer funds
    if (!item->isGold()) {
      buyer->removeGold(price * amount);
      seller->addGold(price * amount);
    }
  }

  // Transfer items
  buyer->addItem(Item::create(item->getItemProfile().jsonFileName), amount);
  seller->removeItem(item, amount);
}

}  // namespace vigilante
