// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Console.h"

namespace vigilante {

Console* Console::_instance = nullptr;

Console* Console::getInstance() {
  if (!_instance) {
    _instance = new Console();
  }
  return _instance;
}

Console::Console() {}




} // namespace vigilante
