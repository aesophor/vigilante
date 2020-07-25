// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_BOT_H_
#define VIGILANTE_BOT_H_

#include <Box2D/Box2D.h>
#include "Character.h"

namespace vigilante {

// Bot interface
class Bot {
 public:
  Bot() = default;
  virtual ~Bot() = default;

 protected:
  virtual void act(float delta) = 0;
  virtual void moveToTarget(Character* target) = 0;
  virtual void moveRandomly(float delta, int minMoveDuration, int maxMoveDuration,
                            int minWaitDuration, int maxWaitDuration) = 0;
  virtual void jumpIfStucked(float delta, float checkInterval) = 0;
  virtual void reverseDirection() = 0;
};

}  // namespace vigilante

#endif  // VIGILANTE_BOT_H_
