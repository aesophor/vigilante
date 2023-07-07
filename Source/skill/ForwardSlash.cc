// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "ForwardSlash.h"

#include "Audio.h"
#include "CallbackManager.h"
#include "character/Character.h"

using namespace std;
USING_NS_AX;

namespace vigilante {

ForwardSlash::ForwardSlash(const string& jsonFileName, Character* user)
    : Skill{},
      _skillProfile(jsonFileName),
      _user(user) {}

void ForwardSlash::import(const string& jsonFileName) {
  _skillProfile = Skill::Profile(jsonFileName);
}

bool ForwardSlash::canActivate() {
  return !_user->isWeaponSheathed()
    && _user->getCharacterProfile().stamina + _skillProfile.deltaStamina >= 0;
}

void ForwardSlash::activate() {
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

  CallbackManager::the().runAfter([=]() {
    _user->getBody()->SetLinearDamping(oldBodyDamping);
    _user->setInvincible(false);
    _user->getFixtures()[Character::FixtureType::BODY]->SetSensor(false);
    _user->removeActiveSkill(this);
  }, _skillProfile.framesDuration);

  // Play sound effect.
  Audio::the().playSfx(_skillProfile.sfxActivate);
}

string ForwardSlash::getIconPath() const {
  return _skillProfile.textureResDir + "/icon.png";
}

} // namespace vigilante
