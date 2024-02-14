// Copyright (c) 2023-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_CHARACTER_NPC_CONTROLLER_H_
#define VIGILANTE_CHARACTER_NPC_CONTROLLER_H_

#include <box2d/box2d.h>

namespace vigilante {

class Character;
class Npc;

class NpcController final {
 public:
  explicit NpcController(Npc& npc) : _npc{npc} {}

  void update(const float delta);

  inline void reverseDirection() { _isMovingRight = !_isMovingRight; }
  inline bool isSandboxing() const { return _isSandboxing; }
  inline void setSandboxing(const bool sandboxing) { _isSandboxing = sandboxing; }
  inline void clearMoveDest() { _moveDest.SetZero(); }

 private:
  void findNewLockedOnTargetFromParty(const Character* killedTarget);
  bool isTooFarAwayFromTarget(const Character* target) const;
  void moveToTarget(const float delta, Character* target, const float followDist);
  void moveToTarget(const float delta, const b2Vec2& targetPos, const float followDist);
  void moveRandomly(const float delta,
                    const int minMoveDuration, const int maxMoveDuration,
                    const int minWaitDuration, const int maxWaitDuration);
  void jumpIfStucked(const float delta, const float checkInterval);

  Npc& _npc;

  bool _isSandboxing{};
  bool _isMovingRight{};
  float _moveDuration{};
  float _moveTimer{};
  float _waitDuration{};
  float _waitTimer{};
  float _calculateDistanceTimer{};
  float _activateSkillTimer{};
  b2Vec2 _moveDest{0.f, 0.f};
  b2Vec2 _lastStoppedPosition{0.f, 0.f};
};

}  // namespace vigilante

#endif  // VIGILANTE_CHARACTER_NPC_CONTROLLER_H_
