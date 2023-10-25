// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "BeastForm.h"

#include <memory>

#include "CallbackManager.h"
#include "character/Character.h"
#include "map/GameMapManager.h"

using namespace std;
USING_NS_AX;

namespace vigilante {

BeastForm::BeastForm(const string& jsonFileName, Character* user)
    : Skill{},
      _skillProfile{jsonFileName},
      _user{user} {}

void BeastForm::import(const string& jsonFileName) {
  _skillProfile = Skill::Profile{jsonFileName};
}

bool BeastForm::canActivate() {
  return _user->getCharacterProfile().magicka + _skillProfile.deltaMagicka >= 0;
}

void BeastForm::activate() {
  _hasActivated = true;
  _user->replaceSpritesheet("Data/character/werewolf.json");
  _user->runIntroAnimation();
}

string BeastForm::getIconPath() const {
  return _skillProfile.textureResDir + "/icon.png";
}

} // namespace vigilante
