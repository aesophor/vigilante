// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "StaticActor.h"

#include <stdexcept>

#include "Constants.h"
#include "map/GameMapManager.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"

using namespace std;
USING_NS_CC;

namespace vigilante {

StaticActor::StaticActor(size_t numAnimations)
    : _isShownOnMap(),
      _bodySprite(),
      _bodySpritesheet(),
      _bodyAnimations(numAnimations) {}

bool StaticActor::showOnMap(float x, float y) {
  if (_isShownOnMap) {
    return false;
  }

  _isShownOnMap = true;
  _bodySprite->setPosition(x, y);

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  gmMgr->getLayer()->addChild(_bodySprite, graphical_layers::kDefault);

  return true;
}

bool StaticActor::removeFromMap() {
  if (!_isShownOnMap) {
    return false;
  }

  _isShownOnMap = false;

  // If _bodySpritesheet exists, we should remove it instead of _bodySprite.
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  gmMgr->getLayer()->removeChild((_bodySpritesheet) ? ((Node*) _bodySpritesheet) : ((Node*) _bodySprite));

  _bodySpritesheet = nullptr;
  _bodySprite = nullptr;
  return true;
}

void StaticActor::setPosition(float x, float y) {
  _bodySprite->setPosition(x, y);
}

Animation* StaticActor::createAnimation(const string& textureResDir,
                                        const string& framesName,
                                        float interval,
                                        Animation* fallback) {
  FileUtils* fileUtils = FileUtils::getInstance();
  SpriteFrameCache* frameCache = SpriteFrameCache::getInstance();

  // The texture resources under Resources/Texture/ has the following rules:
  //
  // Texture/character/player/player_attacking0/0.png
  // |______________________| |____| |________| |___|
  //      textureResDir          |   framesName
  //                      framesNamePrefix
  //
  // As you can see, each framesName (e.g., attacking0) is preceded by a prefix,
  // this is to **prevent frames name collision** in cocos2d::SpriteFrameCache!
  string framesNamePrefix = StaticActor::getLastDirName(textureResDir);

  // Count how many frames (.png) are there in the corresponding directory.
  // Method: we will use FileUtils to test whether a file exists starting from
  //         0.png, 1.png, ..., n.png
  string dir = textureResDir + "/" + framesNamePrefix + "_" + framesName;
  size_t frameCount = 0;
  fileUtils->setPopupNotify(false); // disable CCLOG
  while (fileUtils->isFileExist(dir + "/" + std::to_string(frameCount) + ".png")) {
    frameCount++;
  }
  fileUtils->setPopupNotify(true); // re-enable CCLOG

  // If there are no frames in the corresponding directory, fallback to IDLE_SHEATHED.
  if (frameCount == 0) {
    if (fallback) {
      return fallback;
    } else {
      throw runtime_error("Failed to create animations from " +
                          dir + ", but fallback animation is not provided.");
    }
  }

  Vector<SpriteFrame*> frames;
  for (size_t i = 0; i < frameCount; i++) {
    const string& name = framesNamePrefix + "_" + framesName + "/" +
                         std::to_string(i) + ".png";
    frames.pushBack(frameCache->getSpriteFrameByName(name));
  }
  Animation* animation = Animation::createWithSpriteFrames(frames, interval);
  animation->retain();
  return animation;
}

string StaticActor::getLastDirName(const string& directory) {
  return directory.substr(directory.find_last_of('/') + 1);
}

}  // namespace vigilante
