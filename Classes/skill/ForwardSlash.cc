#include "ForwardSlash.h"

#include "character/Character.h"
#include "map/GameMapManager.h"
#include "util/CallbackUtil.h"

using std::string;

namespace vigilante {

ForwardSlash::ForwardSlash(const string& jsonFileName)
    : _skillProfile(jsonFileName), _hasActivated() {}


void ForwardSlash::import(const string& jsonFileName) {
  _skillProfile = Skill::Profile(jsonFileName);
}

bool ForwardSlash::canActivate(Character* user) {
  return !user->isWeaponSheathed()
    && user->getCharacterProfile().stamina + _skillProfile.deltaStamina >= 0;
}

void ForwardSlash::activate(Character* user) {
  if (_hasActivated) {
    return;
  }

  // Modify character's stats.
  user->getCharacterProfile().stamina += _skillProfile.deltaStamina;

  float rushPower = (user->isFacingRight()) ? 5.0f : -5.0f;
  user->getBody()->SetLinearVelocity({rushPower, 0});

  float oldBodyDamping = user->getBody()->GetLinearDamping();
  user->getBody()->SetLinearDamping(4.0f);

  user->setInvincible(true);
  user->getFixtures()[Character::FixtureType::BODY]->SetSensor(true);

  callback_util::runAfter([=]() {
    user->getBody()->SetLinearDamping(oldBodyDamping);
    user->setInvincible(false);
    user->getFixtures()[Character::FixtureType::BODY]->SetSensor(false);
    delete this;
  }, _skillProfile.framesDuration);
}


Skill::Profile& ForwardSlash::getSkillProfile() {
  return _skillProfile;
}

const string& ForwardSlash::getName() const {
  return _skillProfile.name;
}

const string& ForwardSlash::getDesc() const {
  return _skillProfile.desc;
}

string ForwardSlash::getIconPath() const {
  return _skillProfile.textureResDir + "/icon.png";
}

} // namespace vigilante
