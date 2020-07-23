// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "BatForm.h"

#include <memory>

#include "character/Character.h"
#include "map/GameMapManager.h"
#include "util/CallbackUtil.h"

using std::string;
using std::shared_ptr;
using cocos2d::EventKeyboard;

namespace vigilante {

BatForm::BatForm(const string& jsonFileName, Character* user)
    : Skill(),
      _skillProfile(jsonFileName),
      _user(user),
      _hasActivated() {}


void BatForm::import(const string& jsonFileName) {
  _skillProfile = Skill::Profile(jsonFileName);
}

EventKeyboard::KeyCode BatForm::getHotkey() const {
  return _skillProfile.hotkey;
}

void BatForm::setHotkey(EventKeyboard::KeyCode hotkey) {
  _skillProfile.hotkey = hotkey;
}

bool BatForm::canActivate() {
  return !_user->isWeaponSheathed()
    && _user->getCharacterProfile().stamina + _skillProfile.deltaStamina >= 0;
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

  callback_util::runAfter([=]() {
    _user->getBody()->SetLinearDamping(oldBodyDamping);
    _user->setInvincible(false);
    _user->getFixtures()[Character::FixtureType::BODY]->SetSensor(false);

    shared_ptr<Skill> key(shared_ptr<Skill>(), this);
    _user->getActiveSkills().erase(key);
  }, _skillProfile.framesDuration);
}


Skill::Profile& BatForm::getSkillProfile() {
  return _skillProfile;
}

const string& BatForm::getName() const {
  return _skillProfile.name;
}

const string& BatForm::getDesc() const {
  return _skillProfile.desc;
}

string BatForm::getIconPath() const {
  return _skillProfile.textureResDir + "/icon.png";
}

} // namespace vigilante
