// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Console.h"

#include "util/Logger.h"

using std::string;
using cocos2d::Layer;

namespace vigilante {

Console* Console::_instance = nullptr;

Console* Console::getInstance() {
  if (!_instance) {
    _instance = new Console();
  }
  return _instance;
}

Console::Console() : _layer(Layer::create()), _textField(new TextField()) {
  _layer->setVisible(false);
  _layer->addChild(_textField->getLayout());
}


void Console::update(float delta) {
  if (!_layer->isVisible()) {
    return;
  }
  _textField->update(delta);
}

void Console::handleInput() {
  if (!_layer->isVisible()) {
    return;
  }
  _textField->handleInput();
}

void Console::executeCmd(const string& cmd) {
  VGLOG(LOG_INFO, "Executing: %s", cmd.c_str());
}


Layer* Console::getLayer() const {
  return _layer;
}

} // namespace vigilante
