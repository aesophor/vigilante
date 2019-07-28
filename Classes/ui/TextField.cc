// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "TextField.h"

#include "AssetManager.h"

using cocos2d::Label;
using cocos2d::ui::Layout;
using vigilante::asset_manager::kRegularFont;
using vigilante::asset_manager::kRegularFontSize;

namespace vigilante {

TextField::TextField()
    : _layout(Layout::create()),
      _label(Label::createWithTTF("", kRegularFont, kRegularFontSize)) {
  _layout->addChild(_label);
}


void TextField::handleInput() {

}

Layout* TextField::getLayout() const {
  return _layout;
}

} // namespace vigilante
