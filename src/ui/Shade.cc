// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Shade.h"

#include "AssetManager.h"

using cocos2d::Director;
using cocos2d::FadeOut;
using cocos2d::ui::ImageView;

namespace vigilante {

const float Shade::_kFadeOutTime = .65f;
const float Shade::_kFadeInTime = .3f;

Shade* Shade::getInstance() {
  static Shade instance;
  return &instance;
}

Shade::Shade() : _imageView(ImageView::create(asset_manager::kShade)) { // 1px * 1px
  auto winSize = Director::getInstance()->getWinSize();
  _imageView->setScaleX(winSize.width);
  _imageView->setScaleY(winSize.height);
  _imageView->setAnchorPoint({0, 0});
  _imageView->runAction(FadeOut::create(_kFadeOutTime));
}


ImageView* Shade::getImageView() const {
  return _imageView;
}

}  // namespace vigilante
