// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "CallbackManager.h"

using namespace std;
USING_NS_AX;

namespace vigilante {

CallbackManager& CallbackManager::the() {
  static CallbackManager instance;
  return instance;
}

uint64_t CallbackManager::runAfter(function<void (const CallbackManager::CallbackId)>&& userCallback, float delay) {
  if (delay == 0) {
    userCallback(0);
    return 0;
  }

  const CallbackId id = _nextCallbackId++;
  auto cancellableUserCallback = [this, id, callback = std::move(userCallback)]() {
    const auto it = _cancelledCallbackIds.find(id);
    if (it != _cancelledCallbackIds.end()) {
      _cancelledCallbackIds.erase(it);
      return;
    }
    callback(id);
  };

  _scene->runAction(Sequence::createWithTwoActions(
    DelayTime::create(delay),
    CallFunc::create(cancellableUserCallback)
  ));

  return id;
}

void CallbackManager::cancel(const CallbackManager::CallbackId callbackId) {
  _cancelledCallbackIds.emplace(callbackId);
}

}  // namespace vigilante
