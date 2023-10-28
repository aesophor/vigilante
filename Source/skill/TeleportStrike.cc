// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "TeleportStrike.h"

#include "Audio.h"
#include "CallbackManager.h"
#include "character/Character.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "util/CameraUtil.h"

using namespace std;
USING_NS_AX;

namespace vigilante {

namespace {

bool isTargetFarEnoughFromWall(Character* target, const float minDistRequired, const bool checkBehind) {
  const b2Vec2& targetPos = target->getBody()->GetPosition();
  const float bodyFixtureWidthInHalf = target->getCharacterProfile().bodyWidth / kPpm / 2;
  const float bodyFixtureHeightInHalf = target->getCharacterProfile().bodyHeight / kPpm / 2;

  b2Vec2 src;
  src.x = targetPos.x;
  src.y = targetPos.y - bodyFixtureHeightInHalf + 0.01f;
  if (checkBehind) {
    src.x += target->isFacingRight() ? -bodyFixtureWidthInHalf : bodyFixtureWidthInHalf;
  } else {
    src.x += target->isFacingRight() ? bodyFixtureWidthInHalf : -bodyFixtureWidthInHalf;
  }

  b2Vec2 dst;
  dst.x = src.x;
  dst.y = src.y;
  if (checkBehind) {
    dst.x += target->isFacingRight() ? -minDistRequired : minDistRequired;
  } else {
    dst.x += target->isFacingRight() ? minDistRequired : -minDistRequired;
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  return !gmMgr->rayCast(src, dst, category_bits::kWall | category_bits::kGround);
}

bool isTargetFacingAwayFarEnoughFromWall(Character* target, const float minDistRequired) {
  return isTargetFarEnoughFromWall(target, minDistRequired, /*checkBehind=*/true);
}

bool isTargetFacingAgainstFarEnoughFromWall(Character* target, const float minDistRequired) {
  return isTargetFarEnoughFromWall(target, minDistRequired, /*checkBehind=*/false);
}

}  // namespace

TeleportStrike::TeleportStrike(const string& jsonFileName, Character* user)
    : Skill{},
      _skillProfile{jsonFileName},
      _user{user} {}

void TeleportStrike::import(const string& jsonFileName) {
  _skillProfile = Skill::Profile{jsonFileName};
}

bool TeleportStrike::canActivate() {
  return _user->getCharacterProfile().stamina + _skillProfile.deltaStamina >= 0;
}

void TeleportStrike::activate() {
  if (_hasActivated) {
    return;
  }

  Character* target = getClosestEnemyWithinDist(2.0f);
  if (!target) {
    VGLOG(LOG_ERR, "Failed to activated teleport strike since there are no targets.");
    return;
  }

  _user->getCharacterProfile().stamina += _skillProfile.deltaStamina;

  const b2Vec2 thisPos = _user->getBody()->GetPosition();
  const b2Vec2 targetPos = target->getBody()->GetPosition();
  const optional<b2Vec2> teleportDestPos = determineTeleportDest(target);
  if (!teleportDestPos.has_value()) {
    VGLOG(LOG_ERR, "Failed to activate teleport strike, target [%s] not feasible.", target->getCharacterProfile().name.c_str());
    return;
  }

  CallbackManager::the().runAfter([this, target, targetPos, teleportDestPos](const CallbackManager::CallbackId) {
    _user->teleportToTarget(*teleportDestPos);

    const b2Vec2 thisPos = _user->getBody()->GetPosition();
    _user->setFacingRight(thisPos.x < targetPos.x);

    CallbackManager::the().runAfter([this, target](const CallbackManager::CallbackId) {
      const bool isTargetInRange = _user->getInRangeTargets().contains(target);
      if (!isTargetInRange) {
        _user->getInRangeTargets().insert(target);
      }
      _user->attack(Character::State::ATTACKING_FORWARD, _user->getCharacterProfile().forwardAttackNumTimesInflictDamage);
      if (!isTargetInRange) {
        _user->getInRangeTargets().erase(target);
      }
    }, 0.1f);

    _user->enableAfterImageFx(ax::Color3B{0xa6, 0x53, 0x72});
    CallbackManager::the().runAfter([this](const CallbackManager::CallbackId) {
      _user->disableAfterImageFx();
      _user->getBody()->SetAwake(true);
    }, _user->getAnimationDuration(Character::State::ATTACKING_FORWARD));
  }, _skillProfile.framesDuration);

  Audio::the().playSfx(_skillProfile.sfxActivate);
}

string TeleportStrike::getIconPath() const {
  return _skillProfile.textureResDir + "/icon.png";
}

Character* TeleportStrike::getClosestEnemyWithinDist(const float maxEuclideanDist) const {
  if (_user->getLockedOnTarget() && !_user->getLockedOnTarget()->isSetToKill()) {
    return _user->getLockedOnTarget();
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  float minDist = std::numeric_limits<float>::max();
  Character* target = nullptr;
  for (const auto& dynamicActor : gmMgr->getGameMap()->getDynamicActors()) {
    auto c = dynamic_cast<Character*>(dynamicActor.get());
    if (!c || c->isSetToKill()) {
      continue;
    }

    const b2Vec2& thisPos = _user->getBody()->GetPosition();
    const b2Vec2& targetPos = dynamicActor->getBody()->GetPosition();
    const float dist = std::hypotf(targetPos.x - thisPos.x, targetPos.y - thisPos.y);
    if (dist <= maxEuclideanDist && dist < minDist) {
      minDist = dist;
      target = c;
    }
  }

  return target;
}

std::optional<b2Vec2> TeleportStrike::determineTeleportDest(Character* target) const {
  const float attackRange = _user->getCharacterProfile().attackRange / kPpm;
  const b2Vec2& targetPos = target->getBody()->GetPosition();

  if (isTargetFacingAwayFarEnoughFromWall(target, attackRange)) {
    return b2Vec2{targetPos.x + (target->isFacingRight() ? -attackRange : attackRange), targetPos.y};
  }
  if (isTargetFacingAgainstFarEnoughFromWall(target, attackRange)) {
    return b2Vec2{targetPos.x + (target->isFacingRight() ? attackRange : -attackRange), targetPos.y};
  }
  return std::nullopt;
}

} // namespace vigilante
