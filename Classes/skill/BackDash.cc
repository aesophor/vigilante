// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "BackDash.h"

#include "character/Character.h"
#include "map/GameMapManager.h"
#include "util/CallbackUtil.h"

using std::string;
using cocos2d::EventKeyboard;

namespace vigilante {

BackDash::BackDash(const string& jsonFileName, Character* user)
    : Skill(),
      _skillProfile(jsonFileName),
      _user(user),
      _hasActivated() {}


void BackDash::import(const string& jsonFileName) {
  _skillProfile = Skill::Profile(jsonFileName);
}

EventKeyboard::KeyCode BackDash::getHotkey() const {
  return _skillProfile.hotkey;
}

void BackDash::setHotkey(EventKeyboard::KeyCode hotkey) {
  _skillProfile.hotkey = hotkey;
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

  callback_util::runAfter([=]() {
    _user->getBody()->SetLinearDamping(oldBodyDamping);
    delete this;
  }, _skillProfile.framesDuration);
}


Skill::Profile& BackDash::getSkillProfile() {
  return _skillProfile;
}

const string& BackDash::getName() const {
  return _skillProfile.name;
}

const string& BackDash::getDesc() const {
  return _skillProfile.desc;
}

string BackDash::getIconPath() const {
  return _skillProfile.textureResDir + "/icon.png";
}

} // namespace vigilante
