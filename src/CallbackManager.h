// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_CALLBACK_MANAGER_H_
#define VIGILANTE_CALLBACK_MANAGER_H_

#include <functional>

#include <cocos2d.h>

namespace vigilante {

class CallbackManager {
 public:
  static CallbackManager& the();

  void runAfter(const std::function<void ()>& userCallback, float delay);

  inline void setScene(cocos2d::Scene* scene) { _scene = scene; }

 private:
  CallbackManager() : _scene() {}

  cocos2d::Scene* _scene;
};

}  // namespace vigilante

#endif  // VIGILANTE_CALLBACK_MANAGER_H_
