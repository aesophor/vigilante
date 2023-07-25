// Copyright (c) 2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "AfterImageFxManager.h"

#include "scene/GameScene.h"
#include "scene/SceneManager.h"

using namespace std;
USING_NS_AX;

namespace vigilante {

namespace {

void createAfterImage(const Sprite* sprite,
                      const int zOrder,
                      const Color3B& color,
                      const float durationInSec) {
  Sprite* afterImage = Sprite::createWithSpriteFrame(sprite->getSpriteFrame());
  afterImage->setPosition(sprite->getPosition());
  afterImage->setScale(sprite->getScale());
  afterImage->setRotation(sprite->getRotation());
  afterImage->setVisible(sprite->isVisible());
  afterImage->setLocalZOrder(sprite->getLocalZOrder());
  afterImage->setFlippedX(sprite->isFlippedX());
  afterImage->setColor(color);
  afterImage->setOpacity(80);

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  gmMgr->getLayer()->addChild(afterImage, zOrder);

  DelayTime* delay = DelayTime::create(durationInSec);
  CallFunc* remove = CallFunc::create([=]() {
    afterImage->removeFromParentAndCleanup(true);
  });

  afterImage->runAction(Sequence::create(delay, remove, nullptr));
}

void createAfterImage(const Node* node, const Color3B& color, const float durationInSec) {
  for (const auto child : node->getChildren()) {
    if (const auto spriteBatchNode = dynamic_cast<SpriteBatchNode*>(child)) {
      for (const Sprite* sprite : spriteBatchNode->getDescendants()) {
        createAfterImage(sprite, child->getLocalZOrder() - 1, color, durationInSec);
      }
    }
  }
}

}  // namespace

void AfterImageFxManager::update(float delta) {
  for (auto& [node, afterImageFxData] : _entries) {
    if (afterImageFxData.timerInSec < afterImageFxData.intervalInSec) {
      afterImageFxData.timerInSec += delta;
      continue;
    }

    createAfterImage(node, afterImageFxData.color, afterImageFxData.durationInSec);
    afterImageFxData.timerInSec = 0.0f;
  }
}

bool AfterImageFxManager::registerNode(const Node* node,
                                       const Color3B& color,
                                       const float durationInSec,
                                       const float intervalInSec) {
  const auto it = _entries.find(node);
  if (it != _entries.end()) {
    VGLOG(LOG_ERR, "Failed to register node to AfterImageFxManager, err: [already registered].");
    return false;
  }

  _entries[node] = AfterImageFxData{color, durationInSec, intervalInSec, 0.0f};
  return true;
}

bool AfterImageFxManager::unregisterNode(const ax::Node* node) {
  const auto it = _entries.find(node);
  if (it == _entries.end()) {
    VGLOG(LOG_ERR, "Failed to register node to AfterImageFxManager, err: [node hasn't been registered].");
    return false;
  }

  _entries.erase(it);
  return true;
}

}  // namespace vigilante
