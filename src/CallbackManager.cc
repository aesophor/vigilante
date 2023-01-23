// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "CallbackManager.h"

using namespace std;
USING_NS_CC;

namespace vigilante {

CallbackManager& CallbackManager::the() {
  static CallbackManager instance;
  return instance;
}

void CallbackManager::runAfter(const function<void ()>& userCallback, float delay) {
  // If the specified delay is 0 second, then we can
  // simply invoke `userCallback` and return early.
  if (delay == 0) {
    userCallback();
    return;
  }

  _scene->runAction(Sequence::createWithTwoActions(
    DelayTime::create(delay),
    CallFunc::create(userCallback)
  ));
}

}  // namespace vigilante
