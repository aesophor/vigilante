// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "FxManager.h"

#include "Constants.h"
#include "StaticActor.h"

using cocos2d::Animate;
using cocos2d::Animation;
using cocos2d::CallFunc;
using cocos2d::FileUtils;
using cocos2d::FiniteTimeAction;
using cocos2d::Layer;
using cocos2d::Repeat;
using cocos2d::RepeatForever;
using cocos2d::Sequence;
using cocos2d::Sprite;
using cocos2d::SpriteBatchNode;
using cocos2d::SpriteFrame;
using cocos2d::SpriteFrameCache;
using cocos2d::Vector;
using std::string;

namespace vigilante {

FxManager::FxManager(Layer* gameMapLayer) : _gameMapLayer(gameMapLayer) {}

void FxManager::createFx(const string& textureResDir, const string& framesName, float x,
                         float y) {
  // If the cocos2d::Animation* is not present in cache, then create one
  // and cache this animation object.
  //
  // Texture/fx/dust/dust_white/0.png
  // |_____________| |__||____|
  //  textureResDir    |  framesName
  //            framesNamePrefix
  string framesNamePrefix = StaticActor::getLastDirName(textureResDir);
  string cacheKey = textureResDir + "/" + framesNamePrefix + "_" + framesName;

  if (_animationCache.find(cacheKey) == _animationCache.end()) {
    Animation* animation =
        StaticActor::createAnimation(textureResDir, framesName, 10.0f / kPpm);
    _animationCache.insert({cacheKey, animation});
  }

  // Select the first frame (e.g., dust_white/0.png) as the default look of the sprite.
  Sprite* sprite =
      Sprite::createWithSpriteFrameName(framesNamePrefix + "_" + framesName + "/0.png");
  sprite->setPosition(x, y);

  string spritesheetFileName = FxManager::getSpritesheetFileName(textureResDir);
  SpriteBatchNode* spritesheet = SpriteBatchNode::create(spritesheetFileName);
  spritesheet->addChild(sprite);
  spritesheet->getTexture()->setAliasTexParameters();
  _gameMapLayer->addChild(spritesheet, 80);

  // Run animation.
  FiniteTimeAction* animate = nullptr;
  animate = Repeat::create(Animate::create(_animationCache[cacheKey]), 1);
  auto cleanup = CallFunc::create([=]() { _gameMapLayer->removeChild(spritesheet); });
  sprite->runAction(Sequence::createWithTwoActions(animate, cleanup));
}

string FxManager::getSpritesheetFileName(const string& textureResDir) {
  // Example: Texture/fx/dust/spritesheet.png
  //          |_____________| |_____________|
  //           textureResDir
  return textureResDir + "/spritesheet.png";
}

}  // namespace vigilante
