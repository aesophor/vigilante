// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "StaticActor.h"

#include <filesystem>
#include <stdexcept>

#include "Constants.h"
#include "map/GameMapManager.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "util/AxUtil.h"

namespace fs = std::filesystem;
using namespace std;
USING_NS_AX;

namespace requiem {

bool StaticActor::showOnMap(float x, float y) {
  if (_isShownOnMap) {
    return false;
  }

  _isShownOnMap = true;
  _bodySprite->setPosition(x, y);
  _node->addChild(_bodySprite, z_order::kDefault);

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  ax_util::addChildWithParentCameraMask(gmMgr->getLayer(), _node);

  return true;
}

bool StaticActor::removeFromMap() {
  if (!_isShownOnMap) {
    return false;
  }

  _isShownOnMap = false;

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  gmMgr->getLayer()->removeChild(_node, true);

  _bodySpritesheet = nullptr;
  _bodySprite = nullptr;
  return true;
}

void StaticActor::setPosition(float x, float y) {
  _bodySprite->setPosition(x, y);
}

Animation* StaticActor::createAnimation(const fs::path& textureResDirPath,
                                        const string& framesName,
                                        const float interval,
                                        Animation* fallback) {
  FileUtils* fileUtils = FileUtils::getInstance();
  SpriteFrameCache* frameCache = SpriteFrameCache::getInstance();

  // The texture resources under Resources/Texture/ has the following rules:
  //
  // Texture/character/player/player_attacking0/0.png
  // |______________________| |____| |________| |___|
  //    textureResDirPath        |   framesName
  //                      framesNamePrefix
  //
  // As you can see, each framesName (e.g., attacking0) is preceded by a prefix,
  // this is to **prevent frames name collision** in ax::SpriteFrameCache!
  const string framesNamePrefix = StaticActor::getLastDirName(textureResDirPath);

  // Count how many frames (.png) are there in the corresponding directory.
  // Method: we will use FileUtils to test whether a file exists starting from
  //         0.png, 1.png, ..., n.png
  const fs::path dirPath = textureResDirPath / (framesNamePrefix + "_" + framesName);
  if (!fileUtils->isDirectoryExist(dirPath.native())) {
    return fallback;
  }

  size_t frameCount = 0;
  fileUtils->setPopupNotify(false); // disable CCLOG
  while (fileUtils->isFileExist((dirPath / (std::to_string(frameCount) + ".png")).native())) {
    frameCount++;
  }
  fileUtils->setPopupNotify(true); // re-enable CCLOG

  // If there are no frames in the corresponding directory, fallback to IDLE_SHEATHED.
  if (frameCount == 0) {
    if (fallback) {
      return fallback;
    } else {
      throw runtime_error("Failed to create animations from " +
                          dirPath.native() + ", but fallback animation is not provided.");
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

string StaticActor::getLastDirName(const fs::path& directoryPath) {
  return directoryPath.native().substr(directoryPath.native().find_last_of('/') + 1);
}

fs::path StaticActor::getSpritesheetFilePath(const fs::path& textureResDirPath) {
  return textureResDirPath / "spritesheet.png";
}

}  // namespace requiem
