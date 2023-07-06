// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "HeaderPane.h"

#include "Assets.h"
#include "ui/Colorscheme.h"
#include "ui/pause_menu/PauseMenu.h"

using namespace std;
using namespace vigilante::assets;
USING_NS_AX;

namespace vigilante {

HeaderPane::HeaderPane(PauseMenu* pauseMenu)
    : AbstractPane(pauseMenu),
      _currentIndex() {
  _layout->setLayoutType(ui::Layout::Type::RELATIVE);
  _layout->setAnchorPoint({0, 1});  // Make top-left (0, 0)

  float nextX = 0;
  for (int i = 0; i < PauseMenu::Pane::SIZE; i++) {
    Label* label = Label::createWithTTF(PauseMenu::_kPaneNames[i], string{kTitleFont}, kRegularFontSize);
    label->setTextColor(colorscheme::kGrey);
    label->setPositionX(nextX + _kOptionGap * i);
    label->getFontAtlas()->setAliasTexParameters();
    _layout->addChild(label);

    _labels.push_back(label);
    nextX += label->getContentSize().width;
  }

  select(PauseMenu::Pane::INVENTORY);
}

void HeaderPane::update() {

}

void HeaderPane::handleInput() {

}

void HeaderPane::select(int index) {
  if (index < 0 || index >= _kOptionCount) {
    return;
  }
  _labels[_currentIndex]->setTextColor(colorscheme::kGrey);
  _labels[index]->setTextColor(colorscheme::kWhite);
  _currentIndex = index;
}

void HeaderPane::selectPrev() {
  select((_currentIndex - 1 >= 0) ? _currentIndex - 1 : _kOptionCount - 1);
}

void HeaderPane::selectNext() {
  select((_currentIndex + 1) % _kOptionCount);
}

} // namespace vigilante
