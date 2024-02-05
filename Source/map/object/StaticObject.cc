// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "StaticObject.h"

#include "Assets.h"
#include "Constants.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"

using namespace std;
USING_NS_AX;

namespace vigilante {

bool StaticObject::showOnMap(float x, float y) {
  if (_isShownOnMap) {
    return false;
  }

  _isShownOnMap = true;

  defineTexture();
  _bodySprite->setPosition(x, y);

  _node->removeAllChildren();
  _node->addChild(_bodySpritesheet);

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  gmMgr->getLayer()->addChild(_node, _zOrder);

  return true;
}

void StaticObject::defineTexture() {
  // If the ax::Animation* is not present in cache,
  // then create one and cache this animation object.
  //
  // Texture/fx/dust/dust_white/0.png
  // |_____________| |__||____|
  //  textureResDir    |  framesName
  //            framesNamePrefix
  const string framesNamePrefix = StaticActor::getLastDirName(_textureResDir);

  // Select the first frame (e.g., dust_white/0.png) as the default look of the sprite.
  _bodySprite = Sprite::createWithSpriteFrameName(framesNamePrefix + "_" + _framesName + "/0.png");

  const string spritesheetFileName = StaticActor::getSpritesheetFileName(_textureResDir);
  _bodySpritesheet = SpriteBatchNode::create(spritesheetFileName);
  _bodySpritesheet->addChild(_bodySprite);
  _bodySpritesheet->getTexture()->setAliasTexParameters();

  Animation* animation = StaticActor::createAnimation(_textureResDir, _framesName, _frameInterval / kPpm);
  auto animate = Animate::create(animation);
  _bodySprite->runAction(RepeatForever::create(animate));

  if (_flipped) {
    _bodySprite->setFlippedX(true);
  }
}

}  // namespace vigilante
