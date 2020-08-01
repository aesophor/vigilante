// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "TradeListView.h"

#include "AssetManager.h"

#define VISIBLE_ITEM_COUNT 5
#define WIDTH 289.5
#define HEIGHT 120
#define ITEM_GAP_HEIGHT 25
#define REGULAR_BG vigilante::asset_manager::kItemRegular
#define HIGHLIGHTED_BG vigilante::asset_manager::kItemHighlighted

namespace vigilante {

TradeListView::TradeListView(TradeWindow* tradeWindow)
    : ListView<Item*>(VISIBLE_ITEM_COUNT, WIDTH, HEIGHT, ITEM_GAP_HEIGHT, REGULAR_BG, HIGHLIGHTED_BG),
      _tradeWindow(tradeWindow) {}


void TradeListView::confirm() {

}

void TradeListView::selectUp() {

}

void TradeListView::selectDown() {

}

}  // namespace vigilante
