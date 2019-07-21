// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "DialogueMenu.h"

#include "AssetManager.h"

using cocos2d::Layer;
using cocos2d::ui::ImageView;

namespace vigilante {

DialogueMenu* DialogueMenu::_instance = nullptr;

DialogueMenu* DialogueMenu::getInstance() {
  if (!_instance) {
    _instance = new DialogueMenu();
  }
  return _instance;
}

DialogueMenu::DialogueMenu() : _layer(Layer::create()) {

}


void DialogueMenu::handleInput() {

}

bool DialogueMenu::isVisible() const {
  return _layer->isVisible();
}

void DialogueMenu::setVisible(bool visible) {
  _layer->setVisible(visible);
}

Layer* DialogueMenu::getLayer() const {
  return _layer;
}

} // namespace vigilante
