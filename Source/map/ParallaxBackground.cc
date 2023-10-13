// Copyright (c) 2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "ParallaxBackground.h"

#include "scene/SceneManager.h"

namespace fs = std::filesystem;
using namespace std;
USING_NS_AX;

namespace vigilante {

bool InfiniteParallaxNode::init() {
  _visibleSize = Director::getInstance()->getVisibleSize();
  return true;
}

void InfiniteParallaxNode::addLayer(const string &fileName,
                                    const int z,
                                    const Vec2& parallaxRatio,
                                    const Vec2& position,
                                    const Vec2& scale) {
  auto spriteA = Sprite::create(fileName);
  spriteA->setLocalZOrder(z);
  spriteA->setPosition(position);
  spriteA->setScale(scale.width, scale.height);
  spriteA->getTexture()->setAliasTexParameters();
  addChild(spriteA);

  auto spriteB = Sprite::create(fileName);
  spriteB->setLocalZOrder(z);
  spriteB->setPosition(position);
  spriteB->setScale(scale.width, scale.height);
  spriteB->getTexture()->setAliasTexParameters();
  addChild(spriteB);

  _layerData[fileName] = ParallaxLayerData{
    .ratio = parallaxRatio,
    .position = position,
    .scale = scale,
    .spriteA = spriteA,
    .spriteB = spriteB
  };
}

void InfiniteParallaxNode::update(const float delta) {
  const auto gameCamera = SceneManager::the().getCurrentScene<GameScene>()->getGameCamera();
  if (!gameCamera) {
    VGLOG(LOG_ERR, "GameScene's game camera not present.");
    return;
  }

  const Vec2 gameCameraPosDelta = gameCamera->getPosition() - _prevGameCameraPos;
  for (auto &[_, layerData] : _layerData) {
    const Vec2 layerDelta = gameCameraPosDelta * layerData.ratio;
    layerData.spriteA->setPosition(layerData.spriteA->getPosition() - layerDelta);
    layerData.spriteB->setPosition(layerData.spriteB->getPosition() - layerDelta);

    const float spriteWidth = layerData.spriteA->getContentSize().width * layerData.scale.x;
    const auto posA = layerData.spriteA->getPosition();
    if (posA.x <= 0.5 * _visibleSize.width) {
      const float x = posA.x + spriteWidth - 1;
      const float y = posA.y;
      layerData.spriteB->setPosition(x, y);
    } else {
      const float x = posA.x - spriteWidth + 1;
      const float y = posA.y;
      layerData.spriteB->setPosition(x, y);
    }

    const float limitL = 0.5 * (_visibleSize.width - spriteWidth);
    const float limitR = 0.5 * (_visibleSize.width + spriteWidth);
    if (posA.x < limitL || posA.x > limitR) {
      std::swap(layerData.spriteA, layerData.spriteB);
    }
  }

  _prevGameCameraPos = gameCamera->getPosition();
}

void ParallaxBackground::update(const float delta) {
  if (!_parallaxNode) {
    return;
  }
  _parallaxNode->update(delta);
}

bool ParallaxBackground::load(const fs::path& bgDirPath) {
  error_code ec;
  if (!fs::exists(bgDirPath, ec)) {
    VGLOG(LOG_ERR, "Failed to load parallax background from dir: [%s]", bgDirPath.c_str());
    return false;
  }

  _parallaxNode = InfiniteParallaxNode::create();
  if (!_parallaxNode) {
    VGLOG(LOG_ERR, "Failed to create and initialize infinite parallax node.");
    return false;
  }

  constexpr int kMax = 10;
  for (int i = 0; i < kMax; i++) {
    const string bgFileName = std::to_string(i) + ".png";
    const fs::path bgPath = bgDirPath / bgFileName;
    if (!fs::exists(bgPath, ec)) {
      break;
    }

    const auto& winSize = Director::getInstance()->getWinSize();
    const int z = i;
    const Vec2 parallaxRatio{(1.5f * i) / kPpm, 0};
    const Vec2 position{winSize.width / 2, winSize.height / 2};
    const Vec2 scale{1.4f, 1.4f};
    _parallaxNode->addLayer(bgPath, i, parallaxRatio, position, scale);
  }

  return true;
}

}  // namespace vigilante
