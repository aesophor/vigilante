// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "AssetManager.h"

#include <fstream>
#include <stdexcept>
#include <string>

#include <cocos2d.h>
#include "util/Logger.h"

    using cocos2d::SpriteFrameCache;
using std::ifstream;
using std::runtime_error;
using std::string;

namespace vigilante {

namespace asset_manager {

void loadSpritesheets(const string& spritesheetsListFileName) {
  ifstream fin(spritesheetsListFileName);
  if (!fin.is_open()) {
    throw runtime_error("Failed to load spritesheets from " + spritesheetsListFileName);
  }

  VGLOG(LOG_INFO, "Loading textures...");
  SpriteFrameCache* frameCache = SpriteFrameCache::getInstance();
  string line;
  while (std::getline(fin, line)) {
    if (!line.empty()) {
      frameCache->addSpriteFramesWithFile(line);
    }
  }
}

}  // namespace asset_manager

}  // namespace vigilante
