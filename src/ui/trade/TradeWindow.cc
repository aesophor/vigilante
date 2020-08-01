// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "TradeWindow.h"

namespace vigilante {

TradeWindow::TradeWindow() : _targetNpc() {

}

void TradeWindow::update() {

}

void TradeWindow::handleInput() {

}


Npc* TradeWindow::getTargetNpc() const {
  return _targetNpc;
}

void TradeWindow::setTargetNpc(Npc* targetNpc) {
  _targetNpc = targetNpc;
}

}  // namespace vigilante
