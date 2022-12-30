// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_CALLBACK_MANAGER_H_
#define VIGILANTE_CALLBACK_MANAGER_H_

#include <atomic>
#include <functional>

#include <cocos2d.h>

namespace vigilante {

class CallbackManager {
 public:
  static CallbackManager* getInstance();
  virtual ~CallbackManager() = default;

  void runAfter(const std::function<void ()>& userCallback, float delay);

  inline int getPendingCount() const { return _pendingCount; }
  inline void setScene(cocos2d::Scene* scene) { _scene = scene; }

 private:
  CallbackManager();

  cocos2d::Scene* _scene;
  std::atomic<int> _pendingCount;  // # of callbacks pending to run
};

}  // namespace vigilante

#endif  // VIGILANTE_CALLBACK_MANAGER_H_
