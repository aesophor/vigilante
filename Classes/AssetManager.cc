#include "AssetManager.h"

#include <string>
#include <fstream>
#include <stdexcept>

#include "cocos2d.h"

using std::string;
using std::ifstream;
using std::runtime_error;
using cocos2d::SpriteFrameCache;

namespace vigilante {

namespace asset_manager {

void loadSpritesheets(const string& spritesheetListFileName) {
  ifstream fin(spritesheetListFileName);
  if (!fin.is_open()) {
    throw runtime_error("Failed to load spritesheets from " + spritesheetListFileName);
  }

  cocos2d::log("[AssetManager.cc] loading textures");
  SpriteFrameCache* frameCache = SpriteFrameCache::getInstance();
  string line;
  while (std::getline(fin, line)) {
    if (!line.empty()) {
      frameCache->addSpriteFramesWithFile(line);
    }
  }
}

} // namespace asset_manager

} // namespace vigilante
