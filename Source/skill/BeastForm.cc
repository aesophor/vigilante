// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "BeastForm.h"

#include <memory>

#include "Assets.h"
#include "CallbackManager.h"
#include "character/Character.h"
#include "map/GameMapManager.h"

namespace fs = std::filesystem;
using namespace std;
using namespace vigilante::assets;
USING_NS_AX;

namespace vigilante {

BeastForm::BeastForm(const fs::path& jsonFilePath, Character* user)
    : Skill{},
      _skillProfile{jsonFilePath},
      _user{user} {}

void BeastForm::import(const fs::path& jsonFilePath) {
  _skillProfile = Skill::Profile{jsonFilePath};
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
  _user->replaceSpritesheet(_user->getCharacterProfile().jsonFilePath);
  _user->getCharacterProfile() = _originalCharacterProfile;
}

fs::path BeastForm::getIconPath() const {
  return _skillProfile.textureResDirPath / kIconPng;
}

} // namespace vigilante
