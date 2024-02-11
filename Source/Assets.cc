// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Assets.h"

#include <filesystem>
#include <string>

#include <axmol.h>

#include "util/Logger.h"

using namespace std;
USING_NS_AX;

namespace vigilante::assets {

void loadSpritesheets() {
  VGLOG(LOG_INFO, "Loading textures...");

  SpriteFrameCache* frameCache = SpriteFrameCache::getInstance();
  for (const auto dentry : fs::recursive_directory_iterator{kTextureDir}) {
    if (const string dirPath{fs::path{dentry}}; dirPath.ends_with(".plist")) {
      frameCache->addSpriteFramesWithFile(dirPath);
      VGLOG(LOG_INFO, "Successfully loaded spritesheet [%s]...", dirPath.c_str());
    }
  }
}

}  // namespace vigilante::assets
