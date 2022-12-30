// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "FxManager.h"

#include "Constants.h"
#include "StaticActor.h"
#include "DynamicActor.h"
#include "character/Character.h"
#include "map/GameMapManager.h"

using namespace std;
USING_NS_CC;

namespace vigilante {

FxManager* FxManager::getInstance() {
  static FxManager instance;
  return &instance;
}

void FxManager::createDustFx(Character* c) {
  const b2Vec2& feetPos = c->getBody()->GetPosition();
  float x = feetPos.x * kPpm;
  float y = (feetPos.y - .1f) * kPpm;

  createFx("Texture/fx/dust", "white", x, y, 1, 10);
}

Sprite* FxManager::createHintBubbleFx(const b2Body* body,
                                      const string& framesName) {
  const b2Vec2& bodyPos = body->GetPosition();
  float x = bodyPos.x * kPpm;
  float y = bodyPos.y * kPpm + HINT_BUBBLE_FX_SPRITE_OFFSET_Y;

  return createFx("Texture/fx/hint_bubble", framesName, x, y, -1, 45);
}

Sprite* FxManager::createFx(const string& textureResDir,
                            const string& framesName,
                            float x,
                            float y,
                            unsigned int loopCount,
                            float frameInterval) {
  bool shouldRepeatForever = loopCount == (unsigned int) -1;
  Layer* gameMapLayer = GameMapManager::getInstance()->getLayer();

  // If the cocos2d::Animation* is not present in cache,
  // then create one and cache this animation object.
  //
  // Texture/fx/dust/dust_white/0.png
  // |_____________| |__||____|
  //  textureResDir    |  framesName
  //            framesNamePrefix
  string framesNamePrefix = StaticActor::getLastDirName(textureResDir);
  string cacheKey = textureResDir + "/" + framesNamePrefix + "_" + framesName;

  if (_animationCache.find(cacheKey) == _animationCache.end()) {
    Animation* animation = StaticActor::createAnimation(textureResDir,
                                                        framesName,
                                                        frameInterval / kPpm);
    _animationCache.insert({cacheKey, animation});
  }

  // Select the first frame (e.g., dust_white/0.png) as the default look of the sprite.
  Sprite* sprite = Sprite::createWithSpriteFrameName(framesNamePrefix + "_" +
                                                     framesName + "/0.png");
  sprite->setPosition(x, y);

  string spritesheetFileName = FxManager::getSpritesheetFileName(textureResDir);
  SpriteBatchNode* spritesheet = SpriteBatchNode::create(spritesheetFileName);
  spritesheet->addChild(sprite);
  spritesheet->getTexture()->setAliasTexParameters();
  gameMapLayer->addChild(spritesheet, graphical_layers::kFx);

  // Run animation.
  Animate* animate = Animate::create(_animationCache[cacheKey]);

  if (shouldRepeatForever) {
    sprite->runAction(RepeatForever::create(animate));

  } else {
    auto cleanup = [gameMapLayer, spritesheet]() {
      gameMapLayer->removeChild(spritesheet);
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

string FxManager::getSpritesheetFileName(const string& textureResDir) {
  // Example: Texture/fx/dust/spritesheet.png
  //          |_____________| |_____________|
  //           textureResDir
  return textureResDir + "/spritesheet.png";
}

}  // namespace vigilante
