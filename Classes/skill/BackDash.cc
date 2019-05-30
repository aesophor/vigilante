#include "BackDash.h"

#include "character/Character.h"
#include "map/GameMapManager.h"
#include "util/CallbackUtil.h"

using std::string;

namespace vigilante {

BackDash::BackDash(const string& jsonFileName, Character* user)
    : Skill(),
      _skillProfile(jsonFileName),
      _user(user),
      _hasActivated() {}


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

  float rushPower = (_user->isFacingRight()) ? -3.8f : 3.8f;
  _user->getBody()->SetLinearVelocity({rushPower, .4f});

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

} // namespace vigilante
