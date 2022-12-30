// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "AbstractPane.h"

USING_NS_CC;

namespace vigilante {

AbstractPane::AbstractPane(PauseMenu* pauseMenu)
    : _pauseMenu(pauseMenu), _layout(ui::Layout::create()) {}

AbstractPane::AbstractPane(PauseMenu* pauseMenu, ui::Layout* layout)
    : _pauseMenu(pauseMenu), _layout(layout) {}

bool AbstractPane::isVisible() const {
  if (_layout) {
    return _layout->isVisible();
  }
  return false;
}

void AbstractPane::setVisible(bool visible) const {
  if (_layout) {
    _layout->setVisible(visible);
  }
}

void AbstractPane::setPosition(const Vec2& pos) const {
  _layout->setPosition(pos);
}

} // namespace vigilante
