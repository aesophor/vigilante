// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Shade.h"

#include "Assets.h"

USING_NS_CC;

namespace vigilante {

Shade* Shade::getInstance() {
  static Shade instance;
  return &instance;
}

Shade::Shade() : _imageView(ui::ImageView::create(assets::kShade)) {
  auto winSize = Director::getInstance()->getWinSize();
  _imageView->setScaleX(winSize.width);
  _imageView->setScaleY(winSize.height);
  _imageView->setAnchorPoint({0, 0});
  _imageView->runAction(FadeOut::create(kFadeOutTime));
}

}  // namespace vigilante
