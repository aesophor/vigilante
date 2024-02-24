// Copyright (c) 2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "Lighting.h"

#include "Assets.h"
#include "Constants.h"

using namespace std;
using namespace vigilante::assets;
USING_NS_AX;

namespace vigilante {

Lighting::Lighting()
    : _layer{Layer::create()},
      _darknessOverlay{RenderTexture::create(1, 1)} {
  _layer->addChild(_darknessOverlay);
}

void Lighting::update() {
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

void Lighting::addLightSource(StaticActor *staticActor) {
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

void Lighting::setDarknessOverlaySize(const float width, const float height) const {
  _darknessOverlay->initWithWidthAndHeight(width, height, backend::PixelFormat::RGBA8);
  _darknessOverlay->setCameraMask(_layer->getCameraMask());
  _darknessOverlay->setPosition(width / 2, height / 2);
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

}  // namespace vigilante
