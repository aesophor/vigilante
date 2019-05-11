#include "Bot.h"

#include <set>

#include "util/RandUtil.h"

using std::set;


namespace vigilante {

Bot::Bot(Character* c)
    : _character(c),
      _isMovingRight(),
      _moveDuration(),
      _moveTimer(),
      _waitDuration(),
      _waitTimer(),
      _lastTraveledDistance(),
      _calculateDistanceTimer() {}


void Bot::act(float delta) {
  if (_character->isSetToKill()) {
    return;
  }

  set<Character*>& inRangeTargets = _character->getInRangeTargets();
  Character* lockedOnTarget = _character->getLockedOnTarget();

  if (_character->isAlerted() && lockedOnTarget && !lockedOnTarget->isSetToKill()) {
    if (!inRangeTargets.empty()) { // is target within the attack range?
      _character->attack();
      if (inRangeTargets.empty()) {
        _character->setLockedOnTarget(nullptr);
      }
    } else if (std::abs(_character->getB2Body()->GetPosition().x - lockedOnTarget->getB2Body()->GetPosition().x) > .25f) {
      // If the target isn't within attack range, move toward it until attackable
      moveToTarget(lockedOnTarget);
      jumpIfStucked(delta, .1f);
    }
  } else {
    moveRandomly(delta, 0, 5, 0, 5);
  }
}

void Bot::moveToTarget(Character* target) {
  b2Body* thisBody = _character->getB2Body();
  b2Body* targetBody = target->getB2Body();

  if (thisBody->GetPosition().x > targetBody->GetPosition().x) {
    _character->moveLeft();
  } else {
    _character->moveRight();
  }
}

void Bot::moveRandomly(float delta,
                       int minMoveDuration,
                       int maxMoveDuration,
                       int minWaitDuration,
                       int maxWaitDuration) {
  // If the character has finished moving and waiting, regenerate random values for
  // _moveDuration and _waitDuration within the specified range.
  if (_moveTimer >= _moveDuration && _waitTimer >= _waitDuration) {
    _isMovingRight = static_cast<bool>(rand_util::randInt(0, 1));
    _moveDuration = rand_util::randInt(minMoveDuration, maxMoveDuration);
    _waitDuration = rand_util::randInt(minWaitDuration, maxWaitDuration);
    _moveTimer = 0;
    _waitTimer = 0;
  }

  if (_moveTimer < _moveDuration) {
    if (_isMovingRight) {
      _character->moveRight();
    } else {
      _character->moveLeft();
    }
    // Make sure the character doesn't get stucked somewhere along the way.
    jumpIfStucked(delta, .1f);
    _moveTimer += delta;
  } else {
    _waitTimer += delta;
  }
}

void Bot::jumpIfStucked(float delta, float checkInterval) {
  b2Body* thisBody = _character->getB2Body();

  if (_calculateDistanceTimer > checkInterval) {
    _lastTraveledDistance = std::abs(thisBody->GetPosition().x - _lastStoppedPosition.x);
    _lastStoppedPosition.Set(thisBody->GetPosition().x, thisBody->GetPosition().y);
    if (_lastTraveledDistance == 0) {
      _character->jump();
    }
    _calculateDistanceTimer = 0;
  } else {
    _calculateDistanceTimer += delta;
  }
}

void Bot::reverseDirection() {
  _isMovingRight = !_isMovingRight;
}

} // namespace vigilante
