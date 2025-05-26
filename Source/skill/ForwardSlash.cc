// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "ForwardSlash.h"

#include "Assets.h"
#include "Audio.h"
#include "CallbackManager.h"
#include "character/Character.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "util/CameraUtil.h"

namespace fs = std::filesystem;
using namespace std;
using namespace requiem::assets;
USING_NS_AX;

namespace requiem {

ForwardSlash::ForwardSlash(const fs::path& jsonFilePath, Character* user)
    : Skill{},
      _skillProfile{jsonFilePath},
      _user{user} {}

void ForwardSlash::import(const fs::path& jsonFilePath) {
  _skillProfile = Skill::Profile{jsonFilePath};
}

bool ForwardSlash::canActivate() {
  return _user->getCharacterProfile().stamina + _skillProfile.deltaStamina >= 0;
}

void ForwardSlash::activate() {
  if (_hasActivated) {
    return;
  }

  camera_util::shake(3, .4f);

  // Modify character's stats.
  _user->getCharacterProfile().stamina += _skillProfile.deltaStamina;

  const float rushPower = (_user->isFacingRight()) ? 5.f : -5.f;
  _user->getBody()->SetLinearVelocity({rushPower, 0});

  const float oldBodyDamping = _user->getBody()->GetLinearDamping();
  _user->getBody()->SetLinearDamping(2.f);

  const float oldGravityScale = _user->getBody()->GetGravityScale();
  _user->getBody()->SetGravityScale(0.f);

  _user->setInvincible(true);
  _user->getFixtures()[Character::FixtureType::BODY]->SetSensor(true);

  auto afterImageFxMgr = SceneManager::the().getCurrentScene<GameScene>()->getAfterImageFxManager();
  afterImageFxMgr->registerNode(_user->getNode(), AfterImageFxManager::kPlayerAfterImageColor, 0.15f, 0.05f);

  CallbackManager::the().runAfter([this, oldGravityScale](const CallbackManager::CallbackId) {
    _user->getBody()->SetGravityScale(oldGravityScale);
  }, _skillProfile.framesDuration / 4);

  CallbackManager::the().runAfter([this, oldBodyDamping](const CallbackManager::CallbackId) {
    auto afterImageFxMgr = SceneManager::the().getCurrentScene<GameScene>()->getAfterImageFxManager();
    afterImageFxMgr->unregisterNode(_user->getNode());

    _user->getBody()->SetLinearDamping(oldBodyDamping);
    _user->setInvincible(false);
    _user->getFixtures()[Character::FixtureType::BODY]->SetSensor(false);
    _user->removeActiveSkillInstance(this);
  }, _skillProfile.framesDuration);

  // Play sound effect.
  Audio::the().playSfx(_skillProfile.sfxActivate);
}

fs::path ForwardSlash::getIconPath() const {
  return _skillProfile.textureResDirPath / kIconPng;
}

} // namespace requiem
