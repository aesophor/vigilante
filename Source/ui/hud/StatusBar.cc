// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "StatusBar.h"

using namespace std;
USING_NS_AX;

namespace vigilante {

StatusBar::StatusBar(const string& leftPaddingImgPath,
                     const string& rightPaddingImgPath,
                     const string& statusBarImgPath,
                     const float maxLength,
                     const float numSecAutoHide)
    : _layout{ui::Layout::create()},
      _leftPaddingImg{ui::ImageView::create(leftPaddingImgPath)},
      _rightPaddingImg{ui::ImageView::create(rightPaddingImgPath)},
      _statusBarImg{ui::ImageView::create(statusBarImgPath)},
      _maxLength{maxLength},
      _numSecAutoHide{numSecAutoHide} {
  _leftPaddingImg->setAnchorPoint({0, 0});
  _rightPaddingImg->setAnchorPoint({0, 0});
  _statusBarImg->setAnchorPoint({0, 0});
  _statusBarImg->setScaleX(_maxLength);

  _layout->setLayoutType(ui::Layout::Type::HORIZONTAL);
  _layout->addChild(_leftPaddingImg);
  _layout->addChild(_statusBarImg);
  _layout->addChild(_rightPaddingImg);
}

void StatusBar::update(const float delta) {
  if (_numSecAutoHide == _kNoAutoHide || !isVisible()) {
    return;
  }

  _autoHideTimer += delta;
  if (_autoHideTimer >= _numSecAutoHide) {
    setVisible(false);
    _autoHideTimer = 0.0f;
  }
}

void StatusBar::update(const int currentVal, const int fullVal) {
  _statusBarImg->setScaleX(_maxLength * currentVal / fullVal);
  _rightPaddingImg->setPositionX(_statusBarImg->getPositionX() + _maxLength * currentVal / fullVal);
}

}  // namespace vigilante
