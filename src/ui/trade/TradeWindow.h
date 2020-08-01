// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_TRADE_WINDOW_H_
#define VIGILANTE_TRADE_WINDOW_H_

#include "ui/Window.h"
#include "character/Npc.h"

namespace vigilante {

class TradeWindow : public Window {
 public:
  TradeWindow();
  virtual ~TradeWindow() = default;

  virtual void update() override;  // Window
  virtual void handleInput() override;  // Window

  Npc* getTargetNpc() const;
  void setTargetNpc(Npc* targetNpc);

 private:
  Npc* _targetNpc;
};

}  // namespace vigilante

#endif  // VIGILANTE_TRADE_WINDOW_H_
