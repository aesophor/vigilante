// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "TradeListView.h"

#include <memory>

#include "Assets.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "ui/AmountSelectionWindow.h"
#include "ui/WindowManager.h"
#include "ui/hud/Notifications.h"
#include "ui/trade/TradeWindow.h"
#include "util/StringUtil.h"

using namespace std;
using namespace requiem::assets;
USING_NS_AX;

namespace requiem {

namespace {

constexpr int kVisibleItemCount = 5;
constexpr float kWidth = 289.5f;
constexpr float kHeight = 120.0f;
constexpr float kItemGapHeight = 25.0f;

constexpr float kDescLabelX = 5.0f;
constexpr float kDescLabelY = -132.0f;

}  // namespace

TradeListView::TradeListView(TradeWindow* tradeWindow)
    : ListView<Item*>{kVisibleItemCount, kWidth, kHeight, kItemGapHeight, kItemRegular, kItemHighlighted},
      _tradeWindow{tradeWindow},
      _descLabel{Label::createWithTTF("", string{assets::kRegularFont}, assets::kRegularFontSize)} {
  // _setObjectCallback is called at the end of ListView<T>::ListViewItem::setObject()
  // see ui/ListView.h
  _setObjectCallback = [this](ListViewItem* listViewItem, Item* item) {
    ui::ImageView* icon = listViewItem->getIcon();
    Label* label = listViewItem->getLabel();

    assert(item != nullptr);
    icon->loadTexture(item->getIconPath().native());
    label->setString(item->getName());

    // Display item price if not trading with ally.
    if (!_tradeWindow->isTradingWithAlly()) {
      label->setString(string{label->getString()} + string_util::format(" [$%d]", item->getItemProfile().price));
    }

    // Display item amount if amount > 1
    if (item->getAmount() > 1) {
      label->setString(string{label->getString()} + string_util::format(" (%d)", item->getAmount()));
    }
  };

  _descLabel->getFontAtlas()->setAliasTexParameters();
  _descLabel->setAnchorPoint({0, 1});
  _descLabel->setPosition({kDescLabelX, kDescLabelY});
  _descLabel->setWidth(kWidth - kDescLabelX * 2);
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
    auto window = std::make_unique<AmountSelectionWindow>();
    AmountSelectionWindow* windowRawPtr = window.get();

    auto onSubmit = [windowRawPtr, this, buyer, seller, item]() {
      const string& buf = windowRawPtr->getTextField()->getString();
      int amount = 0;

      auto notifications = SceneManager::the().getCurrentScene<GameScene>()->getNotifications();
      try {
        amount = std::stoi(buf);
      } catch (const std::invalid_argument& ex) {
        notifications->show("Invalid amount");
      } catch (const std::out_of_range& ex) {
        notifications->show("Amount too large or too small");
      } catch (...) {
        notifications->show("Unknown error while parsing amount");
      }

      doTrade(buyer, seller, item, amount);
    };

    auto onDismiss = []() {
      // Close AmountSelectionWindow which
      // should be at the top at this moment.
      auto wm = SceneManager::the().getCurrentScene<GameScene>()->getWindowManager();
      wm->pop();
    };

    window->getTextField()->setOnSubmit(onSubmit);
    window->getTextField()->setOnDismiss(onDismiss);
    window->getTextField()->setReceivingInput(true);

    auto wm = SceneManager::the().getCurrentScene<GameScene>()->getWindowManager();
    wm->push(std::move(window));
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

  auto notifications = SceneManager::the().getCurrentScene<GameScene>()->getNotifications();
  const int price = item->getItemProfile().price;

  // Check if `amount` is valid.
  if (amount > item->getAmount()) {
    notifications->show("Invalid amount");
    return;
  }

  if (!_tradeWindow->isTradingWithAlly()) {
    if (buyer->getGoldBalance() < price * amount) {
      notifications->show("The buyer doesn't have sufficient amount of gold.");
      return;
    }

    if (!item->isGold()) {
      buyer->removeGold(price * amount);
      seller->addGold(price * amount);
    }
  }

  // Transfer items
  buyer->addItem(Item::create(item->getItemProfile().jsonFilePath), amount);
  seller->removeItem(item, amount);
}

}  // namespace requiem
