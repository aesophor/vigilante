#include "GameAssetManager.h"

using std::string;

namespace vigilante {

namespace asset_manager {

string getFrameNamePrefix(const string& spritesheetPath) {
  return spritesheetPath.substr(spritesheetPath.find_last_of('/') + 1);
}

} // namespace asset_manager

} // namespace vigilante
