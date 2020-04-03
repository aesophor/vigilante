// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "OptionListView.h"

#include <cassert>

#include "character/Player.h"
#include "ui/pause_menu/PauseMenu.h"

#define VISIBLE_ITEM_COUNT 5
#define WIDTH vigilante::kVirtualWidth / 2
#define REGULAR_BG vigilante::asset_manager::kItemRegular
#define HIGHLIGHTED_BG vigilante::asset_manager::kItemHighlighted

#define DESC_LABEL_X 5
#define DESC_LABEL_Y -132

using cocos2d::Label;

namespace vigilante {

OptionListView::OptionListView(PauseMenu* pauseMenu)
    : ListView<Option*>(VISIBLE_ITEM_COUNT, WIDTH, REGULAR_BG, HIGHLIGHTED_BG),
      _pauseMenu(pauseMenu) {
  // _setObjectCallback is called at the end of ListView<T>::ListViewItem::setObject()
  // see ui/ListView.h
  _setObjectCallback = [](ListViewItem* listViewItem, Option* option) {
    Label* label = listViewItem->getLabel();
    label->setString(option->first);
  };
}

void OptionListView::confirm() {
  const auto& optionHandler = getSelectedObject()->second;
  optionHandler();
}

}  // namespace vigilante
