// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "OptionListView.h"

#include <cassert>

#include "character/Player.h"
#include "ui/pause_menu/PauseMenu.h"

using namespace vigilante::assets;
USING_NS_AX;

namespace vigilante {

namespace {

constexpr int kVisibleItemCount = 5;
constexpr float kWidth = 289.5f;
constexpr float kHeight = 120.0f;
constexpr float kItemGapHeight = 25.0f;

}  // namespace

OptionListView::OptionListView(PauseMenu* pauseMenu)
    : ListView<Option*>{kVisibleItemCount, kWidth, kHeight, kItemGapHeight, kItemRegular, kItemHighlighted},
      _pauseMenu{pauseMenu} {
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

  dialog->setOption(1, true, "Confirm", [this, &optionHandler]() {
    optionHandler();
    _pauseMenu->update();
  });
  dialog->setOption(2, true, "Cancel");
  dialog->show();
}

}  // namespace vigilante
