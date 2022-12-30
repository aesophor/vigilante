// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "PauseMenuDialog.h"

#include "Assets.h"
#include "input/InputManager.h"
#include "ui/TableLayout.h"
#include "ui/control_hints/ControlHints.h"

using namespace std;
using namespace vigilante::assets;
USING_NS_CC;

namespace vigilante {

PauseMenuDialog::PauseMenuDialog(PauseMenu* pauseMenu)
    : AbstractPane(pauseMenu, TableLayout::create()),
      _message(Label::createWithTTF("", kBoldFont, kRegularFontSize)),
      _current() {
  _message->setAnchorPoint({0, 1});
  _message->getFontAtlas()->setAliasTexParameters();
  _layout->addChild(_message);

  addOption("option1");
  addOption("option2");
  addOption("option3");
}

void PauseMenuDialog::update() {
  const auto& winSize = cocos2d::Director::getInstance()->getWinSize();
  // Automatically position the options :-)
  int count = 1;
  float optionWidth = 0;

  for (const auto& option : _options) {
    optionWidth = std::max(optionWidth, option->getWidth());
  }

  for (int i = _options.size() - 1; i >= 0; i--) {
    if (!_options[i]->isVisible()) {
      continue;
    }
    Option* option = _options[i].get();
    option->getLayout()->setPositionX(winSize.width - 110 - (optionWidth + 15) * count);
    count++;
  }
}

void PauseMenuDialog::handleInput() {
  if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_LEFT_ARROW)) {
    selectLeft();
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_RIGHT_ARROW)) {
    selectRight();
  } else if (IS_KEY_JUST_PRESSED(EventKeyboard::KeyCode::KEY_ENTER)) {
    confirm();
  }
}

void PauseMenuDialog::selectLeft() {
  if (_current == 0 || !_options[_current - 1]->isVisible()) {
    return;
  }
  _options[_current]->setSelected(false);
  _options[--_current]->setSelected(true);
}

void PauseMenuDialog::selectRight() {
  if (_current == (int) _options.size() - 1 || !_options[_current + 1]->isVisible()) {
    return;
  }
  _options[_current]->setSelected(false);
  _options[++_current]->setSelected(true);
}

void PauseMenuDialog::confirm() {
  if (_options.size() == 0) {
    return;
  }
  setVisible(false);
  _options[0]->setSelected(true);
  _options[_current]->setSelected(false);
  _options[_current]->getHandler()(); // invokes a std::function<void ()>

  ControlHints::getInstance()->setVisible(true);
}

void PauseMenuDialog::reset() {
  setMessage("");
  _options[_current]->setSelected(false);
  for (auto& option : _options) {
    option->setVisible(false);
  }
}

void PauseMenuDialog::setMessage(const string& message) const {
  _message->setString(message);
}

void PauseMenuDialog::setOption(int index, bool visible, const string& text, const function<void ()>& handler) const {
  if (index < 0 || index >= 3) {
    return;
  }
  _options[index]->setVisible(visible);
  _options[index]->setText(text);
  _options[index]->setHandler(handler);
}

void PauseMenuDialog::show() {
  update();
  setVisible(true);

  ControlHints::getInstance()->setVisible(false);

  // Set the first visible option as selected.
  for (int i = 0; i < (int) _options.size(); i++) {
    if (_options[i]->isVisible()) {
      _options[i]->setSelected(true);
      _current = i;
      break;
    }
  }
}

void PauseMenuDialog::addOption(const string& text, const function<void ()>& handler) {
  _options.push_back(std::make_unique<Option>(text, handler));
  _layout->addChild(_options.back()->getLayout());
  update();
  _options.front()->setSelected(true);
}

void PauseMenuDialog::clearOptions() {
  for (const auto& option : _options) {
    _layout->removeChild(option->getLayout());
  }
}

PauseMenuDialog::Option::Option(const string& text, const function<void ()>& handler)
    : _layout(ui::Layout::create()),
      _icon(ui::ImageView::create(kDialogueTriangle)),
      _label(Label::createWithTTF(text, kBoldFont, kRegularFontSize)),
      _handler(handler) {
  _icon->setAnchorPoint({0, 1});
  _icon->setVisible(false);

  _label->setAnchorPoint({0, 1});
  _label->getFontAtlas()->setAliasTexParameters();
  _label->setPositionX(_icon->getContentSize().width + _kIconLabelGap);

  _layout->setAnchorPoint({0, 1});
  _layout->addChild(_icon);
  _layout->addChild(_label);
}

float PauseMenuDialog::Option::getWidth() const {
  return _icon->getContentSize().width + _label->getContentSize().width;
}

}  // namespace vigilante
