// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "AmountSelectionWindow.h"

#include "Assets.h"
#include "character/Player.h"
#include "input/InputManager.h"
#include "util/StringUtil.h"

using namespace std;
using namespace vigilante::assets;
USING_NS_AX;

namespace vigilante {

namespace {

constexpr float kMarginLeft = 10.0f;
constexpr float kMarginRight = 10.0f;
constexpr float kMarginTop = 5.0f;
constexpr float kMarginBottom = 15.0f;

}  // namespace

AmountSelectionWindow::AmountSelectionWindow()
    : Window{},
      _contentBackground{ui::ImageView::create(string{kTextFieldBg})},
      _textField{"1"} {

  setTitle("How many?");
  Size windowSize = _titleLabel->getContentSize();
  windowSize.width = std::max(windowSize.width, _contentBackground->getContentSize().width);
  windowSize.height += _contentBackground->getContentSize().height;
  windowSize.width += kMarginLeft;
  windowSize.width += kMarginRight;
  windowSize.height += kMarginTop;
  windowSize.height += kMarginBottom;
  resize(windowSize);

  _contentBackground->setAnchorPoint({0, 1});
  _contentBackground->setPosition({0, 15});

  _contentLayout->setLayoutType(ui::Layout::Type::ABSOLUTE);
  _contentLayout->setAnchorPoint({0, 1});
  _contentLayout->addChild(_contentBackground, 0);

  // Place the text field
  _textField.getLayout()->setPosition({5, 0});
  _contentLayout->addChild(_textField.getLayout(), 1);

  update(0);
}

AmountSelectionWindow::~AmountSelectionWindow() {
  if (_textField.isReceivingInput()) {
    _textField.setReceivingInput(false);
  }
}

void AmountSelectionWindow::update(const float delta) {
  _textField.update(delta);
}

void AmountSelectionWindow::handleInput() {

}

}  // namespace vigilante
