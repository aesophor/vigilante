#include "CharacterProfile.h"

#include <fstream>

#include "cocos2d.h"
#include "json/document.h"

using std::string;
using std::ifstream;
using rapidjson::Document;

namespace vigilante {

CharacterProfile::CharacterProfile(const string& jsonFileName) {
  ifstream fin(jsonFileName);
  if (!fin.is_open()) {
    cocos2d::log("Json file not found: %s", jsonFileName.c_str());
    return;
  }

  string content;
  string line;
  while (std::getline(fin, line)) {
    content += line;
  }
  fin.close();

  Document json;
  json.Parse(content.c_str());

  textureResPath = json["textureResPath"].GetString();
  spriteOffsetX = json["spriteOffsetX"].GetFloat();
  spriteOffsetY = json["spriteOffsetY"].GetFloat();
  spriteScaleX = json["spriteScaleX"].GetFloat();
  spriteScaleY = json["spriteScaleY"].GetFloat();
  frameWidth = json["frameWidth"].GetInt();
  frameHeight = json["frameHeight"].GetInt();

  /*
  frameInterval[0] = json["frameInterval"]["idle_sheathed"].GetFloat();
  frameInterval[1] = json["frameInterval"]["idle_unsheathed"].GetFloat();
  frameInterval[2] = json["frameInterval"]["idle_sheathed"].GetFloat();
  frameInterval[3] = json["frameInterval"]["idle_sheathed"].GetFloat();
  frameInterval[4] = json["frameInterval"]["idle_sheathed"].GetFloat();
  frameInterval[5] = json["frameInterval"]["idle_sheathed"].GetFloat();
  frameInterval[6] = json["frameInterval"]["idle_sheathed"].GetFloat();
  frameInterval[7] = json["frameInterval"]["idle_sheathed"].GetFloat();
  frameInterval[8] = json["frameInterval"]["idle_sheathed"].GetFloat();
  frameInterval[9] = json["frameInterval"]["idle_sheathed"].GetFloat();
  frameInterval[10] = json["frameInterval"]["idle_sheathed"].GetFloat();
  frameInterval[11] = json["frameInterval"]["idle_sheathed"].GetFloat();
  frameInterval[12] = json["frameInterval"]["idle_sheathed"].GetFloat();
  frameInterval[13] = json["frameInterval"]["idle_sheathed"].GetFloat();
  */

  name = json["name"].GetString();
  level = json["level"].GetInt();
  exp = json["exp"].GetInt();
  fullHealth = json["fullHealth"].GetInt();
  fullStamina = json["fullStamina"].GetInt();
  fullMagicka = json["fullMagicka"].GetInt();
  str = json["str"].GetInt();
  dex = json["dex"].GetInt();
  _int = json["int"].GetInt();
  luk = json["luk"].GetInt();

  bodyWidth = json["bodyWidth"].GetInt();
  bodyHeight = json["bodyHeight"].GetInt();
  moveSpeed = json["moveSpeed"].GetFloat();
  jumpHeight = json["jumpHeight"].GetFloat();

  attackForce = json["attackForce"].GetFloat();
  attackTime = json["attackTime"].GetFloat();
  attackRange = json["attackRange"].GetFloat();
  baseMeleeDamage = json["baseMeleeDamage"].GetInt();
}

} // namespace vigilante
