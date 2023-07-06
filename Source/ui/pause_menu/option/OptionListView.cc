// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "OptionListView.h"

#include <cassert>

#include "character/Player.h"
#include "ui/pause_menu/PauseMenu.h"

#define VISIBLE_ITEM_COUNT 5
#define WIDTH 289.5
#define HEIGHT 120
#define ITEM_GAP_HEIGHT 25

#define DESC_LABEL_X 5
#define DESC_LABEL_Y -132

using namespace vigilante::assets;
USING_NS_AX;

namespace vigilante {

OptionListView::OptionListView(PauseMenu* pauseMenu)
    : ListView<Option*>(VISIBLE_ITEM_COUNT, WIDTH, HEIGHT, ITEM_GAP_HEIGHT, kItemRegular, kItemHighlighted),
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
  PauseMenuDialog* dialog = _pauseMenu->getDialog();
  dialog->reset();
  dialog->setMessage("Are you sure?");

  dialog->setOption(1, true, "Confirm", [=]() {
    optionHandler();
    _pauseMenu->update();
  });
  dialog->setOption(2, true, "Cancel");
  dialog->show();
}

} // namespace vigilante
