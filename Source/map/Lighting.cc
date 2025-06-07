// Copyright (c) 2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "Lighting.h"

#include <cmath>

#include "Assets.h"
#include "Constants.h"
#include "map/GameMap.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "util/Logger.h"
#include "util/AxUtil.h"

using namespace std;
using namespace requiem::assets;
USING_NS_AX;

namespace requiem {

Lighting::Lighting() : _layer{Layer::create()} {}

void Lighting::update() {
  const auto inGameTime = SceneManager::the().getCurrentScene<GameScene>()->getInGameTime();
  const float brightnessPercentage = getBrightnessPercentage(inGameTime);
  updateAmbientLightLevel(inGameTime, brightnessPercentage);
  updateParallaxLightLevel(inGameTime, brightnessPercentage);
  updateLightSources();
}

float Lighting::getBrightnessPercentage(const InGameTime* inGameTime) const {
  if (inGameTime->isDay() || inGameTime->isNight()) {
    return 1.0f;  // no lerping needed.
  }

  const int hour = inGameTime->getHour();
  const int minute = inGameTime->getMinute();
  if (inGameTime->isDawn()) {
    const float totalMins = (InGameTime::kDawnEndHour - InGameTime::kDawnBeginHour) * 60;
    const float passedMins = (hour - InGameTime::kDawnBeginHour) * 60 + minute;
    return passedMins / totalMins;
  }
  if (inGameTime->isDusk()) {
    const float totalMins = (InGameTime::kDuskEndHour - InGameTime::kDuskBeginHour) * 60;
    const float passedMins = (hour - InGameTime::kDuskBeginHour) * 60 + minute;
    return 1.0f - passedMins / totalMins;
  }

  VGLOG(LOG_ERR, "Failed to get brightness percentage.");
  return 1.0f;
}

void Lighting::updateAmbientLightLevel(const InGameTime* inGameTime, const float brightnessPercentage) {
  if (inGameTime->isDay()) {
    setAmbientLightLevel(_gameMap->getAmbientLightLevelDay());
    return;
  }
  if (inGameTime->isNight()) {
    setAmbientLightLevel(_gameMap->getAmbientLightLevelNight());
    return;
  }
  if (inGameTime->isDawn()) {
    setAmbientLightLevel(std::lerp(_gameMap->getAmbientLightLevelNight(), _gameMap->getAmbientLightLevelDay(), brightnessPercentage));
    return;
  }
  if (inGameTime->isDusk()) {
    setAmbientLightLevel(std::lerp(_gameMap->getAmbientLightLevelNight(), _gameMap->getAmbientLightLevelDay(), brightnessPercentage));
    return;
  }

  VGLOG(LOG_ERR, "Failed to update ambient light level.");
}

void Lighting::updateParallaxLightLevel(const InGameTime* inGameTime, const float brightnessPercentage) {
  InfiniteParallaxNode* parallaxNode = _gameMap->getParallaxBackground().getParallaxNode();
  if (!parallaxNode) {
    return;
  }

  constexpr uint8_t kMinParallaxBgLightLevel{60};
  constexpr uint8_t kMaxParallaxBgLightLevel{255};

  uint8_t level{};
  if (inGameTime->isDay()) {
    level = kMaxParallaxBgLightLevel;
  } else if (inGameTime->isNight()) {
    level = kMinParallaxBgLightLevel;
  } else if (inGameTime->isDawn() || inGameTime->isDusk()) {
    level = std::lerp(kMinParallaxBgLightLevel, kMaxParallaxBgLightLevel, brightnessPercentage);
  } else {
    VGLOG(LOG_ERR, "Failed to determine brightness level for parallax bg.");
  }

  const Color3B newColor{level, level, level};
  for (const auto child : parallaxNode->getChildren()) {
    if (child->getColor() != newColor) {
      child->setColor(newColor);
    }
  }
}

void Lighting::updateLightSources() {
  _darknessOverlay->beginWithClear(0, 0, 0, 1.f - _ambientLightLevel);

  for (const auto& [dynamicActor, lightSourceSprite] : _dynamicLightSources) {
    lightSourceSprite->setPosition(dynamicActor->getBody()->GetPosition().x * kPpm, dynamicActor->getBody()->GetPosition().y * kPpm);
    lightSourceSprite->visit();
  }

  for (const auto& [pos, lightSourceSprite] : _staticLightSources) {
    lightSourceSprite->setPosition(pos.first, pos.second);
    lightSourceSprite->visit();
  }

  _darknessOverlay->end();
}

void Lighting::addLightSource(DynamicActor* dynamicActor) {
  Sprite* lightSourceSprite = Sprite::create(kLightSource.c_str());
  lightSourceSprite->setBlendFunc({backend::BlendFactor::ZERO, backend::BlendFactor::ONE_MINUS_SRC_ALPHA});
  lightSourceSprite->retain();

  _dynamicLightSources.push_back({dynamicActor, lightSourceSprite});
}

void Lighting::addLightSource(StaticActor* staticActor) {
  Sprite* lightSourceSprite = Sprite::create(kLightSource.c_str());
  lightSourceSprite->setBlendFunc({backend::BlendFactor::ZERO, backend::BlendFactor::ONE_MINUS_SRC_ALPHA});
  lightSourceSprite->retain();

  const float x = staticActor->getBodySprite()->getPosition().x;
  const float y = staticActor->getBodySprite()->getPosition().y;
  _staticLightSources.push_back({{x, y}, lightSourceSprite});
}

void Lighting::addLightSource(const float x, const float y) {
  Sprite* lightSourceSprite = Sprite::create(kLightSource.c_str());
  lightSourceSprite->setBlendFunc({backend::BlendFactor::ZERO, backend::BlendFactor::ONE_MINUS_SRC_ALPHA});
  lightSourceSprite->retain();

  _staticLightSources.push_back({{x, y}, lightSourceSprite});
}

void Lighting::setDarknessOverlaySize(const float width, const float height) {
  _layer->removeChild(_darknessOverlay);

  _darknessOverlay = RenderTexture::create(width, height, backend::PixelFormat::RGBA8);
  _darknessOverlay->setAnchorPoint({0.5, 0.5});
  _darknessOverlay->setPosition(width / 2, height / 2);
  _darknessOverlay->getSprite()->setCameraMask(_layer->getCameraMask());

  ax_util::addChildWithParentCameraMask(_layer, _darknessOverlay);
}

void Lighting::clear() {
  for (const auto& [_, lightSourceSprite] : _dynamicLightSources) {
    lightSourceSprite->release();
  }
  _dynamicLightSources.clear();

  for (const auto& [_, lightSourceSprite] : _staticLightSources) {
    lightSourceSprite->release();
  }
  _staticLightSources.clear();
}

}  // namespace requiem
