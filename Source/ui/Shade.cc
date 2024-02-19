// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "Shade.h"

#include <string>

#include "Assets.h"

using namespace std;
using namespace vigilante::assets;
USING_NS_AX;

namespace vigilante {

Shade::Shade() : _imageView{ui::ImageView::create(string{kShade})} {
  const auto& winSize = Director::getInstance()->getWinSize();
  _imageView->setScale(winSize.width, winSize.height);
  _imageView->setAnchorPoint({0, 0});
  _imageView->runAction(FadeOut::create(kFadeOutTime));
}

}  // namespace vigilante
