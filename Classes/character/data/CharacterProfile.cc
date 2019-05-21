#include "CharacterProfile.h"

#include <fstream>

#include "cocos2d.h"
#include "json/document.h"

#include "character/Character.h"

using std::string;
using std::ifstream;
using rapidjson::Document;

namespace vigilante {

void CharacterProfile::import(const string& jsonFileName) {
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
  cocos2d::log("json: %s", content.c_str());

  Document json;
  json.Parse(content.c_str());

  textureResPath = json["textureResPath"].GetString();
  spriteOffsetX = json["spriteOffsetX"].GetFloat();
  spriteOffsetY = json["spriteOffsetY"].GetFloat();
  spriteScaleX = json["spriteScaleX"].GetFloat();
  spriteScaleY = json["spriteScaleY"].GetFloat();
  frameWidth = json["frameWidth"].GetInt();
  frameHeight = json["frameHeight"].GetInt();

  for (int i = 0; i < Character::State::SIZE; i++) {
    frameInterval[i] = json["frameInterval"][Character::_kCharacterStateStr[i].c_str()].GetFloat();
  }

  name = json["name"].GetString();
  level = json["level"].GetInt();
  exp = json["exp"].GetInt();
  fullHealth = json["fullHealth"].GetInt();
  fullStamina = json["fullStamina"].GetInt();
  fullMagicka = json["fullMagicka"].GetInt();
  health = json["health"].GetInt();
  stamina = json["stamina"].GetInt();
  magicka = json["magicka"].GetInt();
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
