// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "BeastForm.h"

#include <memory>

#include "Assets.h"
#include "CallbackManager.h"
#include "character/Character.h"
#include "map/GameMapManager.h"

using namespace std;
using namespace vigilante::assets;
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

  CallbackManager::the().runAfter([this](const CallbackManager::CallbackId) {
    _originalCharacterProfile = _user->getCharacterProfile();
    _user->replaceSpritesheet("Data/character/werewolf.json");
    _user->runIntroAnimation();
  }, _skillProfile.framesDuration);
}

void BeastForm::deactivate() {
  _user->replaceSpritesheet(_user->getCharacterProfile().jsonFileName);
  _user->getCharacterProfile() = _originalCharacterProfile;
}

string BeastForm::getIconPath() const {
  return fs::path{_skillProfile.textureResDir} / kIconPng;
}

} // namespace vigilante
