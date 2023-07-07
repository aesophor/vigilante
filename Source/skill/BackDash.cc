// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "BackDash.h"

#include <memory>

#include "CallbackManager.h"
#include "character/Character.h"
#include "map/GameMapManager.h"

using namespace std;
USING_NS_AX;

namespace vigilante {

BackDash::BackDash(const string& jsonFileName, Character* user)
    : Skill{},
      _skillProfile{jsonFileName},
      _user{user} {}

void BackDash::import(const string& jsonFileName) {
  _skillProfile = Skill::Profile(jsonFileName);
}

bool BackDash::canActivate() {
  return !_user->isWeaponSheathed() && !_user->isJumping();
}

void BackDash::activate() {
  if (_hasActivated) {
    return;
  }

  float dashPower = (_user->isFacingRight()) ? -3.8f : 3.8f;
  _user->getBody()->SetLinearVelocity({dashPower, .6f});

  float oldBodyDamping = _user->getBody()->GetLinearDamping();
  _user->getBody()->SetLinearDamping(4.0f);

  CallbackManager::the().runAfter([=]() {
    _user->getBody()->SetLinearDamping(oldBodyDamping);
    _user->removeActiveSkill(this);
  }, _skillProfile.framesDuration);
}

string BackDash::getIconPath() const {
  return _skillProfile.textureResDir + "/icon.png";
}

} // namespace vigilante
