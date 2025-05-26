// Copyright (c) 2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "TimeLocationInfo.h"

#include "Assets.h"
#include "Constants.h"
#include "map/GameMapManager.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "util/StringUtil.h"

using namespace std;
using namespace requiem::assets;
USING_NS_AX;

namespace requiem {

namespace {

constexpr auto kOffsetX = -43;
constexpr auto kOffsetY = -43;

}  // namespace

TimeLocationInfo::TimeLocationInfo()
    : _layer{Layer::create()},
      _locationNameLabel{Label::createWithTTF("", string{kRegularFont}, kRegularFontSize)} {
  _locationNameLabel->getFontAtlas()->setAliasTexParameters();
  _locationNameLabel->setAnchorPoint({1, 0.5});

  const float x = ax::Director::getInstance()->getWinSize().width + kOffsetX;
  const float y = ax::Director::getInstance()->getWinSize().height + kOffsetY;
  _layer->setPosition(x, y);
  _layer->addChild(_locationNameLabel);
}

void TimeLocationInfo::update() {
  auto inGameTime = SceneManager::the().getCurrentScene<GameScene>()->getInGameTime();
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();

  const string s = string_util::format("%s | %02d:%02d",
                                       gmMgr->getGameMap()->getLocationName().c_str(),
                                       inGameTime->getHour(), inGameTime->getMinute());
  _locationNameLabel->setString(s);
}

}  // namespace requiem
