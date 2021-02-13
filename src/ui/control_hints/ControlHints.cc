// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "ControlHints.h"

#include <algorithm>

#include "AssetManager.h"
#include "util/KeyCodeUtil.h"
#include "util/Logger.h"

#define CONTROL_HINTS_Y 48
#define CONTROL_HINTS_RIGHT_PADDING_X 64
#define CONTROL_HINTS_MAX_ITEMS 3

using std::string;
using cocos2d::Vec2;
using cocos2d::Size;
using cocos2d::Layer;
using cocos2d::Label;
using cocos2d::Director;
using cocos2d::EventKeyboard;
using cocos2d::ui::Layout;
using cocos2d::ui::ImageView;
using vigilante::asset_manager::kRegularFont;
using vigilante::asset_manager::kRegularFontSize;
using vigilante::asset_manager::kControlHints;

namespace vigilante {

const int ControlHints::_kHintGap = 25;
const int ControlHints::Hint::_kIconLabelGap = 4;

ControlHints* ControlHints::getInstance() {
  static ControlHints instance;
  return &instance;
}

ControlHints::ControlHints()
    : _layer(Layer::create()),
      _hints() {
  _layer->setPositionY(CONTROL_HINTS_Y);
}


bool ControlHints::isShown(const EventKeyboard::KeyCode keyCode) const {
  return std::find_if(_hints.begin(),
                      _hints.end(),
                      [keyCode](const ControlHints::Hint& hint) {
                          return keyCode == hint.getKeyCode();
                      }) != _hints.end();
}

void ControlHints::show(const EventKeyboard::KeyCode keyCode, const string& text) {
  if (_hints.size() >= CONTROL_HINTS_MAX_ITEMS) {
    VGLOG(LOG_WARN, "Unable to add more control hints! Currently have %d.",
          CONTROL_HINTS_MAX_ITEMS);
    return;
  }

  if (isShown(keyCode)) {
    return;
  }

  _hints.push_back(ControlHints::Hint(keyCode, text));
  _layer->addChild(_hints.back().getLayout());
  _layer->setCameraMask(_layer->getCameraMask());

  normalize();
}

void ControlHints::hide(const EventKeyboard::KeyCode keyCode) {
  if (_hints.empty()) {
    VGLOG(LOG_WARN, "Unable to remove any more control hints! Currently have 0.");
    return;
  }

  _hints.erase(std::remove_if(_hints.begin(),
                              _hints.end(),
                              [this, keyCode](const ControlHints::Hint& hint) {
                                  return keyCode == hint.getKeyCode() &&
                                    (_layer->removeChild(hint.getLayout()), true);
                              }),
               _hints.end());

  normalize();
}

void ControlHints::normalize() {
  if (_hints.empty()) {
    return;
  }

  const auto& winSize = Director::getInstance()->getWinSize();
  float nextX = winSize.width - CONTROL_HINTS_RIGHT_PADDING_X;

  for (int i = _hints.size() - 1; i >= 0; i--) {
    nextX -= _hints.at(i).getContentSize().width;
    nextX -= _kHintGap;
    _hints.at(i).getLayout()->setPositionX(nextX);
  }
}


Layer* ControlHints::getLayer() const {
  return _layer;
}


ControlHints::Hint::Hint(const EventKeyboard::KeyCode keyCode, const string& text)
    : _layout(Layout::create()),
      _icon(ImageView::create(kControlHints + keycode_util::keyCodeToString(keyCode) + ".png")),
      _label(Label::createWithTTF(text, kRegularFont, kRegularFontSize)),
      _keyCode(keyCode) {
  _icon->setAnchorPoint({0, 1});
  _label->setAnchorPoint({0, 1});
  _label->setPositionX(_icon->getContentSize().width + _kIconLabelGap);
  _label->getFontAtlas()->setAliasTexParameters();

  _layout->setLayoutType(Layout::Type::HORIZONTAL);
  _layout->addChild(_icon);
  _layout->addChild(_label);
}


Size ControlHints::Hint::getContentSize() const {
  const Size& iconSize = _icon->getContentSize();
  const Size& labelSize = _label->getContentSize();
  return {iconSize.width + labelSize.width, std::max(iconSize.height, labelSize.height)};
}

Layout* ControlHints::Hint::getLayout() const {
  return _layout;
}

EventKeyboard::KeyCode ControlHints::Hint::getKeyCode() const {
  return _keyCode;
}

}  // namespace vigilante
