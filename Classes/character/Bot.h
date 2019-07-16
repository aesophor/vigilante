// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_BOT_H_
#define VIGILANTE_BOT_H_

#include "Box2D/Box2D.h"

#include "Character.h"

namespace vigilante {

class Bot {
 public:
  Bot(Character* c);
  virtual ~Bot() = default;

  virtual void act(float delta);
  void moveToTarget(Character* target);
  void moveRandomly(float delta, int minMoveDuration, int maxMoveDuration, int minWaitDuration, int maxWaitDuration);
  void jumpIfStucked(float delta, float checkInterval);
  void reverseDirection();
  
 private:
  Character* _character;

  // The following variables are used in BotActions::moveRandomly()
  bool _isMovingRight;
  float _moveDuration;
  float _moveTimer;
  float _waitDuration;
  float _waitTimer;

  // The following variables are used in BotActions::jumpIfStucked()
  b2Vec2 _lastStoppedPosition;
  float _lastTraveledDistance;
  float _calculateDistanceTimer;
};

} // namespace vigilante

#endif // VIGILANTE_BOT_H_
