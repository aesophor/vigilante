// Copyright (c) 2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "NpcController.h"

#include "Constants.h"
#include "character/Npc.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "util/RandUtil.h"

using namespace std;
USING_NS_AX;

namespace vigilante {

namespace {

constexpr float kAllyTeleportDist = 2.5f;
constexpr float kAllyFollowDist = .75f;
constexpr float kMoveDestFollowDist = .2f;
constexpr float kJumpCheckInterval = .5f;

}  // namespace

// This Npc may perform one of the following actions:
// (1) Has `_lockedOnTarget` and `_lockedOnTarget` is not dead yet:
//     a. target is within attack range -> attack()
//     b. target not within attack range -> moveToTarget()
// (2) Has `_lockedOnTarget` but `_lockedOnTarget` is dead:
//     a. target belongs to a party -> try to select other member as new _lockedOnTarget
//     b. target doesnt belong to any party -> clear _lockedOnTarget
// (3) If too far away from the party leader, teleport to the leader.
// (4) Has a target destination (_moveDest) to travel to
// (5) Is following another Character -> moveToTarget()
// (6) Sandboxing (just moving around wasting its time) -> moveRandomly()
void NpcController::update(const float delta) {
  if (_npc.isKilled() || _npc.isSetToKill() || _npc.isAttacking()) {
    return;
  }

  Character* lockedOnTarget = _npc.getLockedOnTarget();
  if (lockedOnTarget && !lockedOnTarget->isSetToKill()) {
    if (!lockedOnTarget->getInRangeTargets().empty()) {
      _npc.attack();
    } else {
      moveToTarget(delta, lockedOnTarget, _npc.getCharacterProfile().attackRange / kPpm);
    }
  } else if (lockedOnTarget && lockedOnTarget->isSetToKill()) {
    Character* killedTarget = lockedOnTarget;
    _npc.setLockedOnTarget(nullptr);
    findNewLockedOnTargetFromParty(killedTarget);
  } else if (_npc.getParty() &&
             !_npc.isWaitingForPartyLeader() &&
             isTooFarAwayFromTarget(_npc.getParty()->getLeader())) {
    clearMoveDest();
    _npc.teleportToTarget(_npc.getParty()->getLeader());
  } else if (_moveDest.x || _moveDest.y) {
    moveToTarget(delta, _moveDest, kMoveDestFollowDist);
  } else if (_npc.getParty() && !_npc.isWaitingForPartyLeader()) {
    moveToTarget(delta, _npc.getParty()->getLeader(), kAllyFollowDist);
  } else if (_isSandboxing) {
    moveRandomly(delta, 0, 5, 0, 5);
  }
}

void NpcController::findNewLockedOnTargetFromParty(const Character* killedTarget) {
  if (!killedTarget->getParty()) {
    return;
  }

  for (auto member : killedTarget->getParty()->getLeaderAndMembers()) {
    if (!member->isSetToKill()) {
      _npc.setLockedOnTarget(member);
      return;
    }
  }
}

bool NpcController::isTooFarAwayFromTarget(const Character* target) const {
  const b2Vec2& thisPos = _npc.getBody()->GetPosition();
  const b2Vec2& targetPos = target->getBody()->GetPosition();

  return std::hypotf(targetPos.x - thisPos.x, targetPos.y - thisPos.y) > kAllyTeleportDist;
}

void NpcController::moveToTarget(const const float delta, Character* target, const float followDist) {
  if (!target->getBody()) {
    VGLOG(LOG_WARN, "Unable to move to target: %s (b2body missing)",
                    target->getCharacterProfile().name.c_str());
    return;
  }

  moveToTarget(delta, target->getBody()->GetPosition(), followDist);
}

void NpcController::moveToTarget(const const float delta, const b2Vec2& targetPos, const float followDist) {
  const b2Vec2& thisPos = _npc.getBody()->GetPosition();
  if (std::hypotf(targetPos.x - thisPos.x, targetPos.y - thisPos.y) <= followDist) {
    _moveDest.SetZero();
    return;
  }

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  PathFinder* pathFinder = gmMgr->getGameMap()->getPathFinder();
  if (auto nextHop = pathFinder->findOptimalNextHop(thisPos, targetPos, followDist)) {
    _moveDest = *nextHop;
  } else if (std::abs(targetPos.x - thisPos.x) > .2f) {
    (thisPos.x > targetPos.x) ? _npc.moveLeft() : _npc.moveRight();
  }

  // Sometimes when two Npcs are too close to each other,
  // they will stuck in the same place, unable to attack each other.
  // This is most likely because they are facing at the wrong direction.
  _npc.setFacingRight(targetPos.x > thisPos.x);

  jumpIfStucked(delta, kJumpCheckInterval);
}

void NpcController::moveRandomly(const float delta,
                                 const int minMoveDuration, const int maxMoveDuration,
                                 const int minWaitDuration, const int maxWaitDuration) {
  // If the character has finished moving and waiting, regenerate random values for
  // _moveDuration and _waitDuration within the specified range.
  if (_moveTimer >= _moveDuration && _waitTimer >= _waitDuration) {
    _isMovingRight = static_cast<bool>(rand_util::randInt(0, 1));
    _moveDuration = rand_util::randInt(minMoveDuration, maxMoveDuration);
    _waitDuration = rand_util::randInt(minWaitDuration, maxWaitDuration);
    _moveTimer = 0;
    _waitTimer = 0;
  }

  if (_moveTimer >= _moveDuration) {
    _waitTimer += delta;
  } else {
    _moveTimer += delta;
    (_isMovingRight) ? _npc.moveRight() : _npc.moveLeft();
    jumpIfStucked(delta, /*checkInterval=*/.5f);
  }
}

void NpcController::jumpIfStucked(const const float delta, const float checkInterval) {
  // If we haven't reached checkInterval yet, add delta to the timer
  // and return at once.
  if (_calculateDistanceTimer <= checkInterval) {
    _calculateDistanceTimer += delta;
    return;
  }

  // We've reached checkInterval, so we can make this character jump
  // if it hasn't moved at all, and then reset the timer.
  if (std::abs(_npc.getBody()->GetPosition().x - _lastStoppedPosition.x) == 0) {
    _npc.jump();
  }

  _lastStoppedPosition = {_npc.getBody()->GetPosition().x, _npc.getBody()->GetPosition().y};
  _calculateDistanceTimer = 0;
}

}  // namespace vigilante
