// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "BatForm.h"

#include <memory>

#include "Assets.h"
#include "CallbackManager.h"
#include "character/Character.h"
#include "map/GameMapManager.h"

namespace fs = std::filesystem;
using namespace std;
using namespace requiem::assets;
USING_NS_AX;

namespace requiem {

BatForm::BatForm(const fs::path& jsonFilePath, Character* user)
    : Skill{},
      _skillProfile{jsonFilePath},
      _user{user} {}

void BatForm::import(const fs::path& jsonFilePath) {
  _skillProfile = Skill::Profile{jsonFilePath};
}

bool BatForm::canActivate() {
  return _user->getCharacterProfile().stamina + _skillProfile.deltaStamina >= 0;
}

void BatForm::activate() {
  if (_hasActivated) {
    return;
  }

  // Modify character's stats.
  _user->getCharacterProfile().stamina += _skillProfile.deltaStamina;

  float rushPower = (_user->isFacingRight()) ? 5.0f : -5.0f;
  _user->getBody()->SetLinearVelocity({rushPower, 0});

  float oldBodyDamping = _user->getBody()->GetLinearDamping();
  _user->getBody()->SetLinearDamping(4.0f);

  _user->setInvincible(true);
  _user->getFixtures()[Character::FixtureType::BODY]->SetSensor(true);

  CallbackManager::the().runAfter([this, oldBodyDamping](const CallbackManager::CallbackId) {
    _user->getBody()->SetLinearDamping(oldBodyDamping);
    _user->setInvincible(false);
    _user->getFixtures()[Character::FixtureType::BODY]->SetSensor(false);
    _user->removeActiveSkillInstance(this);
  }, _skillProfile.framesDuration);
}

fs::path BatForm::getIconPath() const {
  return _skillProfile.textureResDirPath / kIconPng;
}

} // namespace requiem
