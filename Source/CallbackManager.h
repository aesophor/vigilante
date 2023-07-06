// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_CALLBACK_MANAGER_H_
#define VIGILANTE_CALLBACK_MANAGER_H_

#include <functional>

#include <axmol.h>

namespace vigilante {

class CallbackManager {
 public:
  static CallbackManager& the();

  void runAfter(const std::function<void ()>& userCallback, float delay);

  inline void setScene(ax::Scene* scene) { _scene = scene; }

 private:
  CallbackManager() : _scene() {}

  ax::Scene* _scene;
};

}  // namespace vigilante

#endif  // VIGILANTE_CALLBACK_MANAGER_H_
