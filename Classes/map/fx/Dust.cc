#include "Dust.h"

#include <string>

#include "util/CallbackUtil.h"
#include "util/Constants.h"

using std::string;
using cocos2d::Layer;
using cocos2d::Vector;
using cocos2d::Sequence;
using cocos2d::CallFunc;
using cocos2d::Animate;
using cocos2d::Animation;
using cocos2d::Sprite;
using cocos2d::SpriteBatchNode;
using cocos2d::SpriteFrame;
using cocos2d::SpriteFrameCache;

namespace vigilante {

Dust::Dust(Layer* gameMapLayer, float x, float y) {
  SpriteFrameCache* frameCache = SpriteFrameCache::getInstance();
  SpriteBatchNode* spritesheet = SpriteBatchNode::create("Texture/FX/Dust/dust.png");
 
  Vector<SpriteFrame*> frames;
  for (size_t i = 0; i < 5; i++) {
    string name = "dust" + std::to_string(i) + ".png";
    frames.pushBack(frameCache->getSpriteFrameByName(name));
  }
  Animation* animation = Animation::createWithSpriteFrames(frames, .1f);

  Sprite* sprite = Sprite::createWithSpriteFrame(frames.front());
  sprite->setPosition(x * kPpm, y * kPpm);

  spritesheet->addChild(sprite);
  spritesheet->getTexture()->setAliasTexParameters();

  // Add it to the gameMapLayer.
  gameMapLayer->addChild(spritesheet, 80);

  // Run animation
  auto animatedAction = Animate::create(animation);
  auto callback = CallFunc::create([=]() {
    gameMapLayer->removeChild(spritesheet);
  });
  sprite->runAction(Sequence::createWithTwoActions(animatedAction, callback));
}

} // namespace vigilante
