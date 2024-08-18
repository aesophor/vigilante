// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "Assets.h"

#include <filesystem>
#include <string>

#include <axmol.h>

#include "util/Logger.h"

namespace fs = std::filesystem;
using namespace std;
USING_NS_AX;

namespace vigilante::assets {

void loadSpritesheets() {
  VGLOG(LOG_INFO, "Loading textures...");

  SpriteFrameCache* frameCache = SpriteFrameCache::getInstance();
  for (const fs::directory_entry& dentry : fs::recursive_directory_iterator{kTextureDir}) {
    if (dentry.is_regular_file() && dentry.path().native().ends_with(".plist")) {
      frameCache->addSpriteFramesWithFile(dentry.path().native());
      VGLOG(LOG_INFO, "Successfully loaded spritesheet [%s]...", dentry.path().c_str());
    }
  }
}

}  // namespace vigilante::assets
