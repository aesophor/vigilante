// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "AmountSelectionWindow.h"

#include "Assets.h"
#include "character/Player.h"
#include "input/InputManager.h"
#include "util/StringUtil.h"

#define AMOUNT_SELECTION_WINDOW_CONTENT_MARGIN_LEFT 10
#define AMOUNT_SELECTION_WINDOW_CONTENT_MARGIN_RIGHT 10
#define AMOUNT_SELECTION_WINDOW_CONTENT_MARGIN_TOP 5
#define AMOUNT_SELECTION_WINDOW_CONTENT_MARGIN_BOTTOM 15

using namespace std;
using namespace vigilante::assets;
USING_NS_AX;

namespace vigilante {

AmountSelectionWindow::AmountSelectionWindow()
    : Window(),
      _contentBackground(ui::ImageView::create(string{kTextFieldBg})),
      _textField("1") {

  setTitle("How many?");
  Size windowSize = _titleLabel->getContentSize();
  windowSize.width = std::max(windowSize.width, _contentBackground->getContentSize().width);
  windowSize.height += _contentBackground->getContentSize().height;
  windowSize.width += AMOUNT_SELECTION_WINDOW_CONTENT_MARGIN_LEFT;
  windowSize.width += AMOUNT_SELECTION_WINDOW_CONTENT_MARGIN_RIGHT;
  windowSize.height += AMOUNT_SELECTION_WINDOW_CONTENT_MARGIN_TOP;
  windowSize.height += AMOUNT_SELECTION_WINDOW_CONTENT_MARGIN_BOTTOM;
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
