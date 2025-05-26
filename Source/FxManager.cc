// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "FxManager.h"

#include "Assets.h"
#include "Constants.h"
#include "StaticActor.h"
#include "DynamicActor.h"
#include "character/Character.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "util/AxUtil.h"

using namespace std;
using namespace requiem::assets;
USING_NS_AX;

namespace requiem {

void FxManager::createDustFx(const Character* c) {
  if (!c) {
    return;
  }

  const b2Vec2& bodyPos = c->getBody()->GetPosition();
  const float x = bodyPos.x * kPpm;
  const float y = (bodyPos.y - .1f) * kPpm;
  createAnimation(kDustDir, "white", x, y, 1, 10);
}

void FxManager::createHitFx(const Character* c) {
  if (!c) {
    return;
  }

  const b2Vec2& bodyPos = c->getBody()->GetPosition();
  const float x = bodyPos.x * kPpm;
  const float y = bodyPos.y * kPpm;
  createAnimation(kHitDir, "normal", x, y, 1, 4);
}

Sprite* FxManager::createHintBubbleFx(const b2Body* body,
                                      const string& framesName) {
  if (!body) {
    return;
  }

  const b2Vec2& bodyPos = body->GetPosition();
  const float x = bodyPos.x * kPpm;
  const float y = bodyPos.y * kPpm + kHintBubbleFxSpriteOffsetY;
  return createAnimation(kHintBubbleDir, framesName, x, y, -1, 45);
}

Sprite* FxManager::createAnimation(const fs::path& textureResDirPath,
                                   const string& framesName,
                                   const float x,
                                   const float y,
                                   const unsigned int loopCount,
                                   const float frameInterval) {
  // If the ax::Animation* is not present in cache,
  // then create one and cache this animation object.
  //
  // Texture/fx/dust/dust_white/0.png
  // |_____________| |__||____|
  // textureResDirPath |  framesName
  //            framesNamePrefix
  const string framesNamePrefix = StaticActor::getLastDirName(textureResDirPath);
  const fs::path cacheKey = textureResDirPath / (framesNamePrefix + "_" + framesName);

  if (_animationCache.find(cacheKey) == _animationCache.end()) {
    Animation* animation = StaticActor::createAnimation(textureResDirPath, framesName, frameInterval / kPpm);
    _animationCache.emplace(cacheKey, animation);
  }

  // Select the first frame (e.g., dust_white/0.png) as the default look of the sprite.
  Sprite* sprite = Sprite::createWithSpriteFrameName(framesNamePrefix + "_" +
                                                     framesName + "/0.png");
  sprite->setPosition(x, y);

  string spritesheetFilePath = StaticActor::getSpritesheetFilePath(textureResDirPath);
  SpriteBatchNode* spritesheet = SpriteBatchNode::create(spritesheetFilePath);
  spritesheet->addChild(sprite);
  spritesheet->getTexture()->setAliasTexParameters();

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  ax_util::addChildWithParentCameraMask(gmMgr->getLayer(), spritesheet, z_order::kFx);

  const bool shouldRepeatForever = loopCount == static_cast<unsigned int>(-1);
  auto animate = Animate::create(_animationCache[cacheKey]);
  if (shouldRepeatForever) {
    sprite->runAction(RepeatForever::create(animate));
  } else {
    auto cleanup = [gmMgr, spritesheet]() {
      gmMgr->getLayer()->removeChild(spritesheet);
    };
    sprite->runAction(Sequence::createWithTwoActions(
        Repeat::create(animate, loopCount),
        CallFunc::create(cleanup)
      )
    );
  }

  return sprite;
}

void FxManager::removeFx(Sprite* sprite) {
  sprite->stopAllActions();
  sprite->removeFromParent();
}

}  // namespace requiem
