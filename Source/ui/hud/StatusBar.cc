// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "StatusBar.h"

using namespace std;
USING_NS_AX;

namespace vigilante {

StatusBar::StatusBar(const string& leftPaddingImgPath,
                     const string& rightPaddingImgPath,
                     const string& statusBarImgPath,
                     float maxLength)
    : _layout(ui::Layout::create()),
      _leftPaddingImg(ui::ImageView::create(leftPaddingImgPath)),
      _rightPaddingImg(ui::ImageView::create(rightPaddingImgPath)),
      _statusBarImg(ui::ImageView::create(statusBarImgPath)),
      _maxLength(maxLength) {
  _leftPaddingImg->setAnchorPoint({0, 0});
  _rightPaddingImg->setAnchorPoint({0, 0});
  _statusBarImg->setAnchorPoint({0, 0});
  _statusBarImg->setScaleX(_maxLength);

  _layout->setLayoutType(ui::Layout::Type::HORIZONTAL);
  _layout->addChild(_leftPaddingImg);
  _layout->addChild(_statusBarImg);
  _layout->addChild(_rightPaddingImg);
}

void StatusBar::update(int currentVal, int fullVal) {
  _statusBarImg->setScaleX(_maxLength * currentVal / fullVal);
  _rightPaddingImg->setPositionX(_statusBarImg->getPositionX() + _maxLength * currentVal / fullVal);
}

}  // namespace vigilante
