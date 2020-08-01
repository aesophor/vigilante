// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "TradeWindow.h"

#include "std/make_unique.h"
#include "AssetManager.h"
#include "input/InputManager.h"
#include "util/StringUtil.h"

#define TRADE_WINDOW_WIDTH 400
#define TRADE_WINDOW_HEIGHT 250

#define TRADE_WINDOW_CONTENT_MARGIN_LEFT 10
#define TRADE_WINDOW_CONTENT_MARGIN_RIGHT 10
#define TRADE_WINDOW_CONTENT_MARGIN_TOP 30
#define TRADE_WINDOW_CONTENT_MARGIN_BOTTOM 30

using cocos2d::ui::Layout;
using cocos2d::ui::ImageView;
using cocos2d::EventKeyboard;
using vigilante::asset_manager::kTradeBg;
using vigilante::asset_manager::kTabRegular;
using vigilante::asset_manager::kTabHighlighted;

namespace vigilante {

TradeWindow::TradeWindow(Character* buyer, Character* seller)
    : Window(TRADE_WINDOW_WIDTH, TRADE_WINDOW_HEIGHT),
      _contentBackground(ImageView::create(kTradeBg)),
      _tabView(std::make_unique<TabView>(kTabRegular, kTabHighlighted)),
      _tradeListView(std::make_unique<TradeListView>(this)),
      _buyer(buyer),
      _seller(seller) {
  // Resize window: Make the window slightly larger than `_contentBackground`.
  auto tradeWindowSize = _contentBackground->getContentSize();
  tradeWindowSize.width += TRADE_WINDOW_CONTENT_MARGIN_LEFT + TRADE_WINDOW_CONTENT_MARGIN_RIGHT;
  tradeWindowSize.height += TRADE_WINDOW_CONTENT_MARGIN_TOP + TRADE_WINDOW_CONTENT_MARGIN_BOTTOM;
  setSize(tradeWindowSize);

  _contentBackground->setAnchorPoint({0, 1});

  _contentLayout->setLayoutType(Layout::Type::ABSOLUTE);
  _contentLayout->setAnchorPoint({0, 1});
  _contentLayout->addChild(_contentBackground, 0);

  // Place two tabs: the first one shows Npc's inventory/goods,
  // the second one shows player's inventory.
  _tabView->addTab("EQUIP");
  _tabView->addTab("USE");
  _tabView->addTab("MISC");
  _tabView->selectTab(0);
  _contentLayout->addChild(_tabView->getLayout());

  // Place trade list view.
  _tradeListView->getLayout()->setPosition({5, -5});
  _contentLayout->addChild(_tradeListView->getLayout());

  // Set window title.
  setTitle(string_util::format(
        "Trading with: %s", seller->getCharacterProfile().name.c_str()));

  update();
}

void TradeWindow::update() {
  Item::Type selectedItemType = static_cast<Item::Type>(_tabView->getSelectedTab()->getIndex());
  _tradeListView->showCharactersItemByType(_buyer, selectedItemType);
}

void TradeWindow::handleInput() {
  auto inputMgr = InputManager::getInstance();

  if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_LEFT_ARROW)) {
    _tabView->selectPrev();
    update();
  } else if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_RIGHT_ARROW)) {
    _tabView->selectNext();
    update();
  } else if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_UP_ARROW)) {
    _tradeListView->selectUp();
  } else if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
    _tradeListView->selectDown();
  } else if (inputMgr->isKeyJustPressed(EventKeyboard::KeyCode::KEY_ENTER)) {

  }
}


Character* TradeWindow::getBuyer() const {
  return _buyer;
}

Character* TradeWindow::getSeller() const {
  return _seller;
}

}  // namespace vigilante
