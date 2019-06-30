#include "BackDash.h"

#include "character/Character.h"
#include "map/GameMapManager.h"
#include "util/CallbackUtil.h"

using std::string;

namespace vigilante {

BackDash::BackDash(const string& jsonFileName)
    : _skillProfile(jsonFileName), _hasActivated() {}


void BackDash::import(const string& jsonFileName) {
  _skillProfile = Skill::Profile(jsonFileName);
}

bool BackDash::canActivate(Character* user) {
  return !user->isWeaponSheathed() && !user->isJumping();
}

void BackDash::activate(Character* user) {
  if (_hasActivated) {
    return;
  }

  float dashPower = (user->isFacingRight()) ? -3.8f : 3.8f;
  user->getBody()->SetLinearVelocity({dashPower, .6f});

  float oldBodyDamping = user->getBody()->GetLinearDamping();
  user->getBody()->SetLinearDamping(4.0f);

  callback_util::runAfter([=]() {
    user->getBody()->SetLinearDamping(oldBodyDamping);
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
