// Copyright (c) 2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "LocationInfo.h"

#include "Assets.h"
#include "Constants.h"
#include "map/GameMapManager.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"

#define HUD_X ax::Director::getInstance()->getWinSize().width - 43
#define HUD_Y ax::Director::getInstance()->getWinSize().height - 43

using namespace std;
using namespace vigilante::assets;
USING_NS_AX;

namespace vigilante {

LocationInfo::LocationInfo()
    : _layer{Layer::create()},
      _locationNameLabel{Label::createWithTTF("", string{kRegularFont}, kRegularFontSize)} {
  _locationNameLabel->getFontAtlas()->setAliasTexParameters();
  _locationNameLabel->setAnchorPoint({1, 0.5});

  _layer->setPosition(HUD_X, HUD_Y);
  _layer->addChild(_locationNameLabel);
}

void LocationInfo::updateLocationName() {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  _locationNameLabel->setString(gmMgr->getGameMap()->getLocationName());
}

}  // namespace vigilante
