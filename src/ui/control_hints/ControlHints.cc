// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "ControlHints.h"

#include <algorithm>
#include <cassert>

#include "AssetManager.h"
#include "util/KeyCodeUtil.h"
#include "util/Logger.h"

#define CONTROL_HINTS_Y 32
#define CONTROL_HINTS_RIGHT_PADDING_X 64
#define CONTROL_HINTS_MAX_ITEMS 3

using std::array;
using std::string;
using std::vector;
using cocos2d::Vec2;
using cocos2d::Size;
using cocos2d::Color4B;
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

const int ControlHints::_kHintGap = 16;
const int ControlHints::Hint::_kIconLabelGap = 4;

ControlHints* ControlHints::getInstance() {
  static ControlHints instance;
  return &instance;
}

ControlHints::ControlHints()
    : _layer(Layer::create()),
      _profiles(),
      _currentProfileStack({ControlHints::Profile::GAME}) {
  _layer->setPositionY(CONTROL_HINTS_Y);

  // Install Control Hints presets.
  _profiles[ControlHints::Profile::PAUSE_MENU_INVENTORY] = {
    {{EventKeyboard::KeyCode::KEY_CAPITAL_Q, EventKeyboard::KeyCode::KEY_CAPITAL_E},
     "Prev / Next", colorscheme::kWhite},
  };

  _profiles[ControlHints::Profile::PAUSE_MENU_EQUIPMENT] = {
  };

  _profiles[ControlHints::Profile::PAUSE_MENU_SKILLS] = {
  };

  _profiles[ControlHints::Profile::PAUSE_MENU_QUESTS] = {
  };

  _profiles[ControlHints::Profile::PAUSE_MENU_OPTIONS] = {
  };

  for (auto profile : _profiles) {
    for (auto& hint : profile) {
      hint.getLayout()->setVisible(false);
      _layer->addChild(hint.getLayout());
    }
  }
}


ControlHints::Profile ControlHints::getCurrentProfile() const {
  return _currentProfileStack.top();
}

void ControlHints::switchToProfile(ControlHints::Profile profile) {
  popProfile();
  pushProfile(profile);
}

void ControlHints::pushProfile(ControlHints::Profile profile) {
  hideAll();
  _currentProfileStack.push(profile);
  showAll();
}

void ControlHints::popProfile() {
  hideAll();
  _currentProfileStack.pop();
  showAll();
}


bool ControlHints::isShown(const vector<EventKeyboard::KeyCode>& keyCodes) {
  const auto& hints = getCurrentProfileHints();

  return std::find_if(hints.begin(),
                      hints.end(),
                      [keyCodes](const ControlHints::Hint& hint) {
                          return keyCodes == hint.getKeyCodes();
                      }) != hints.end();
}

void ControlHints::insert(const vector<EventKeyboard::KeyCode>& keyCodes,
                          const string& text,
                          const Color4B& textColor) {
  auto& hints = getCurrentProfileHints();

  if (hints.size() >= CONTROL_HINTS_MAX_ITEMS) {
    VGLOG(LOG_WARN, "Unable to add more control hints! Currently have %d.",
          CONTROL_HINTS_MAX_ITEMS);
    return;
  }

  if (isShown(keyCodes)) {
    return;
  }

  hints.push_back({keyCodes, text, textColor});
  _layer->addChild(hints.back().getLayout());
  _layer->setCameraMask(_layer->getCameraMask());

  normalize();
}

void ControlHints::remove(const vector<EventKeyboard::KeyCode>& keyCodes) {
  auto& hints = getCurrentProfileHints();

  if (hints.empty()) {
    VGLOG(LOG_WARN, "Unable to remove any more control hints! Currently have 0.");
    return;
  }

  hints.erase(std::remove_if(hints.begin(),
                             hints.end(),
                             [this, keyCodes](const ControlHints::Hint& hint) {
                                 return keyCodes == hint.getKeyCodes() &&
                                   (_layer->removeChild(hint.getLayout()), true);
                             }),
               hints.end());

  normalize();
}

void ControlHints::normalize() {
  auto& hints = getCurrentProfileHints();

  if (hints.empty()) {
    return;
  }

  const auto& winSize = Director::getInstance()->getWinSize();
  float nextX = winSize.width - CONTROL_HINTS_RIGHT_PADDING_X;

  for (int i = hints.size() - 1; i >= 0; i--) {
    nextX -= hints.at(i).getContentSize().width;
    nextX -= _kHintGap;
    hints.at(i).getLayout()->setPositionX(nextX);
  }
}

void ControlHints::showAll() {
  for (auto& hint : getCurrentProfileHints()) {
    hint.getLayout()->setVisible(true);
  }

  normalize();
}

void ControlHints::hideAll() {
  for (auto& hint : getCurrentProfileHints()) {
    hint.getLayout()->setVisible(false);
  }
}


vector<ControlHints::Hint>& ControlHints::getCurrentProfileHints() {
  return _profiles.at(_currentProfileStack.top());
}


bool ControlHints::isVisible() const {
  return _layer->isVisible();
}

void ControlHints::setVisible(bool visible) {
  _layer->setVisible(visible);
}

Layer* ControlHints::getLayer() const {
  return _layer;
}


ControlHints::Hint::Hint(const vector<EventKeyboard::KeyCode>& keyCodes,
                         const string& text,
                         const Color4B& textColor)
    : _layout(Layout::create()),
      _icons(keyCodes.size()),
      _label(Label::createWithTTF(text, kRegularFont, kRegularFontSize)),
      _keyCodes(keyCodes) {

  assert(!keyCodes.empty());
  _layout->setLayoutType(Layout::Type::HORIZONTAL);

  for (size_t i = 0; i < keyCodes.size(); i++) {
    // Example: Texture/ui/control_hints/E.png
    string iconPath = kControlHints +
                      keycode_util::keyCodeToString(keyCodes.at(i)) + ".png";

    _icons[i] = ImageView::create(iconPath);
    _icons[i]->setAnchorPoint({0, 1});
    _icons[i]->setPositionX(i * _icons[i]->getContentSize().width);
    _layout->addChild(_icons[i]);
  }
  
  _label->setAnchorPoint({0, 1});
  _label->setTextColor(textColor);
  _label->setPositionX(_icons.size() * _icons.front()->getContentSize().width +
                       _kIconLabelGap);
  _label->getFontAtlas()->setAliasTexParameters();
  _layout->addChild(_label);
}


Size ControlHints::Hint::getContentSize() const {
  Size ret(0, 0);

  for (auto icon : _icons) {
    const Size& iconSize = icon->getContentSize();
    ret.width += iconSize.width;
    ret.height = std::max(ret.height, iconSize.height);
  }

  const Size& labelSize = _label->getContentSize();
  ret.width += labelSize.width;
  ret.height = std::max(ret.height, labelSize.height);
  return ret;
}

Layout* ControlHints::Hint::getLayout() const {
  return _layout;
}

const vector<EventKeyboard::KeyCode>& ControlHints::Hint::getKeyCodes() const {
  return _keyCodes;
}

}  // namespace vigilante
