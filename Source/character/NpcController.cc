// Copyright (c) 2023-2025 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "NpcController.h"

#include "Constants.h"
#include "character/Npc.h"
#include "combat/CombatMotion.h"
#include "combat/ComboSystem.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "util/RandUtil.h"

using namespace std;
USING_NS_AX;

namespace requiem {

namespace {

constexpr float kAllyTeleportDist = 2.5f;
constexpr float kAllyFollowDist = .75f;
constexpr float kMoveDestFollowDist = .2f;
constexpr float kJumpCheckInterval = .5f;
constexpr float kActivateRandomSkillInterval = 3.0f;

}  // namespace

NpcController::NpcController(Npc& npc)
    : _npc{npc},
      _pathFinder{std::make_unique<AStarPathFinder>()} {}

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
    auto& skillbook = _npc.getSkillBook()[Skill::Type::MAGIC];
    if (skillbook.size() && _activateSkillTimer >= kActivateRandomSkillInterval) {
      Skill* skill = skillbook.front();
      _npc.activateSkill(skill);
      _activateSkillTimer = 0;
    } else if (_npc.getInRangeTargets().contains(lockedOnTarget)) {
      const optional<Character::State> attackState = _npc.getCombatSystem().determineNextAttackState();
      if (!attackState.has_value()) {
        _npc.attack();
        return;
      }
      if (!handleCombatMotion(_npc, *attackState)) {
        VGLOG(LOG_ERR, "Failed to handle combat motion, character: [%s], attackState: [%d]",
              _npc.getCharacterProfile().jsonFilePath.c_str(), *attackState);
      }
    } else if (!_npc.isUsingSkill()) {
      moveToTarget(delta, lockedOnTarget, _npc.getCharacterProfile().attackRange / kPpm);
    }
    _activateSkillTimer += delta;
  } else if (lockedOnTarget && lockedOnTarget->isSetToKill()) {
    Character* killedTarget = lockedOnTarget;
    _npc.setLockedOnTarget(nullptr);
    findNewLockedOnTargetFromParty(killedTarget);
  /*
  } else if (_npc.getParty() &&
             !_npc.isWaitingForPartyLeader() &&
             isTooFarAwayFromTarget(_npc.getParty()->getLeader())) {
    clearMoveDest();
    _npc.teleportToTarget(_npc.getParty()->getLeader());
    _npc.getBody()->SetAwake(true);
  */
  } else if (_moveDest.x || _moveDest.y) {
    moveToTarget(delta, _moveDest, kMoveDestFollowDist);
  } else if (_npc.getParty() && !_npc.isWaitingForPartyLeader()) {
    moveToTarget(delta, _npc.getParty()->getLeader(), kAllyFollowDist);
  } else if (_isSandboxing) {
    moveRandomly(delta, 0, 5, 0, 5);
  }
}

void NpcController::setMoveDest(const b2Vec2& targetPos, function<void()> onArrivalAtTarget) {
  _moveDest = targetPos;
  _onArrivalAtMoveDest = std::move(onArrivalAtTarget);
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

void NpcController::moveToTarget(const float delta, Character* target, const float followDist) {
  if (!target->getBody()) {
    VGLOG(LOG_WARN, "Unable to move to target: %s (b2body missing)",
                    target->getCharacterProfile().name.c_str());
    return;
  }

  moveToTarget(delta, target->getBody()->GetPosition(), followDist);
}

void NpcController::moveToTarget(const float delta, const b2Vec2& targetPos, const float followDist) {
  const b2Vec2& thisPos = _npc.getBody()->GetPosition();

  if (!_moveDest.x && !_moveDest.y) {
    const optional<b2Vec2> waypoint = _pathFinder->getNextWaypoint(thisPos, targetPos, followDist);
    if (!waypoint.has_value()) {
      return;
    }

    auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
    Vec2 tileCoord = gmMgr->getGameMap()->getNavTiledMap().getTileCoordinate(Vec2{waypoint->x * kPpm, waypoint->y * kPpm});
    Vec2 tileOrigin = gmMgr->getGameMap()->getNavTiledMap().getTilePos(tileCoord); // in pixels, probably top-left or bottom-left depending on orientation

    Vec2 p1 = tileOrigin + Vec2(-8, 8);   // top-left
    Vec2 p2 = tileOrigin + Vec2(8, 8);    // top-right
    Vec2 p3 = tileOrigin + Vec2(8, -8);   // bottom-right
    Vec2 p4 = tileOrigin + Vec2(-8, -8);  // bottom-left

    auto draw = ax::DrawNode::create();
    draw->drawRect(p1, p2, p3, p4, ax::Color4F::GREEN, 1);
    ax_util::addChildWithParentCameraMask(gmMgr->getLayer(), draw, 100);

    CallbackManager::the().runAfter([draw](const CallbackManager::CallbackId) {
      SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager()->getLayer()->removeChild(draw);
    }, 5.0f);

    _moveDest = waypoint.value();
  }

  VGLOG(LOG_INFO, "thisPos: [%f, %f], targetPos: [%f, %f], waypoint: [%f,%f]",
        thisPos.x, thisPos.y, targetPos.x, targetPos.y, _moveDest.x, _moveDest.y);

  if (std::hypotf(_moveDest.x - thisPos.x, _moveDest.y - thisPos.y) <= followDist) {
    if (_onArrivalAtMoveDest) {
      std::invoke(_onArrivalAtMoveDest);
      _onArrivalAtMoveDest = nullptr;
    }
    _moveDest.SetZero();
    return;
  }
  
  const float kMoveThreshold = static_cast<float>(16) / kPpm;
  if (_moveDest.x < thisPos.x - kMoveThreshold) {
    _npc.moveLeft();
  } else if (_moveDest.x > thisPos.x + kMoveThreshold) {
    _npc.moveRight();
  }
  if (_moveDest.y < thisPos.y - kMoveThreshold) {
    _npc.jumpDown();
  } else if (_moveDest.y > thisPos.y + kMoveThreshold) {
    _npc.doubleJump();
  }

  // Sometimes when two Npcs are too close to each other,
  // they will stuck in the same place, unable to attack each other.
  // This is most likely because they are facing at the wrong direction.
  //_npc.setFacingRight(targetPos.x > thisPos.x);

  //jumpIfStucked(delta, kJumpCheckInterval);
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

void NpcController::jumpIfStucked(const float delta, const float checkInterval) {
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

}  // namespace requiem
