// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "TradeWindow.h"

#include "Assets.h"
#include "character/Player.h"
#include "input/InputManager.h"
#include "util/StringUtil.h"

#define TRADE_WINDOW_CONTENT_MARGIN_LEFT 10
#define TRADE_WINDOW_CONTENT_MARGIN_RIGHT 10
#define TRADE_WINDOW_CONTENT_MARGIN_TOP 30
#define TRADE_WINDOW_CONTENT_MARGIN_BOTTOM 40

using namespace std;
using namespace vigilante::assets;
USING_NS_CC;

namespace vigilante {

TradeWindow::TradeWindow(Character* buyer, Character* seller)
    : Window(),
      _contentBackground(ui::ImageView::create(kTradeBg)),
      _tabView(make_unique<TabView>(kTabRegular, kTabHighlighted)),
      _tradeListView(make_unique<TradeListView>(this)),
      _isTradingWithAlly(seller->getAllies().find(buyer) != seller->getAllies().end()),
      _buyer(buyer),
      _seller(seller) {
  // Resize window: Make the window slightly larger than `_contentBackground`.
  auto tradeWindowSize = _contentBackground->getContentSize();
  tradeWindowSize.width += TRADE_WINDOW_CONTENT_MARGIN_LEFT + TRADE_WINDOW_CONTENT_MARGIN_RIGHT;
  tradeWindowSize.height += TRADE_WINDOW_CONTENT_MARGIN_TOP + TRADE_WINDOW_CONTENT_MARGIN_BOTTOM;
  resize(tradeWindowSize);

  _contentBackground->setAnchorPoint({0, 1});

  _contentLayout->setLayoutType(ui::Layout::Type::ABSOLUTE);
  _contentLayout->setAnchorPoint({0, 1});
  _contentLayout->addChild(_contentBackground, 0);

  // Place two tabs: the first one shows Npc's inventory/goods,
  // the second one shows player's inventory.
  _tabView->getLayout()->setAnchorPoint({0, 1});
  _tabView->addTab("EQUIP");
  _tabView->addTab("USE");
  _tabView->addTab("MISC");
  _tabView->selectTab(0);
  _contentLayout->addChild(_tabView->getLayout());

  // Place trade list view.
  _tradeListView->getLayout()->setPosition({5, -5});
  _contentLayout->addChild(_tradeListView->getLayout());

  update(0);
}

void TradeWindow::update(float) {
  if (_isTradingWithAlly) {
    setTitle((dynamic_cast<Player*>(_buyer)) ?
        string_util::format("Receiving from: %s", _seller->getCharacterProfile().name.c_str()) :
        string_util::format("Giving to: %s", _buyer->getCharacterProfile().name.c_str())
    );
  } else {
    setTitle((dynamic_cast<Player*>(_buyer)) ?
        string_util::format("Buying from: %s", _seller->getCharacterProfile().name.c_str()) :
        string_util::format("Selling to: %s", _buyer->getCharacterProfile().name.c_str())
    );
  }

  Item::Type selectedItemType = static_cast<Item::Type>(_tabView->getSelectedTab()->getIndex());
  _tradeListView->showCharactersItemByType(_seller, selectedItemType);
}

void TradeWindow::handleInput() {
  if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_Q) ||
      IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_E)) {
    toggleBuySell();
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_LEFT_ARROW)) {
    _tabView->selectPrev();
    update(0);
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_RIGHT_ARROW)) {
    _tabView->selectNext();
    update(0);
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_UP_ARROW)) {
    _tradeListView->selectUp();
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
    _tradeListView->selectDown();
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_ENTER)) {
    _tradeListView->confirm();
  }
}

void TradeWindow::toggleBuySell() {
  std::swap(_buyer, _seller);
  update(0);
}

}  // namespace vigilante
