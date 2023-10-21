// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_CALLBACK_MANAGER_H_
#define VIGILANTE_CALLBACK_MANAGER_H_

#include <cstdint>
#include <functional>
#include <unordered_set>

#include <axmol.h>

namespace vigilante {

class CallbackManager {
 public:
  using CallbackId = uint64_t;
  static CallbackManager& the();

  CallbackId runAfter(std::function<void ()>&& userCallback, float delay);
  void cancel(const CallbackId id);

  inline void setScene(ax::Scene* scene) { _scene = scene; }

 private:
  CallbackManager() : _scene() {}

  static inline CallbackId _nextCallbackId{1};

  ax::Scene* _scene;
  std::unordered_set<CallbackId> _cancelledCallbackIds;
};

}  // namespace vigilante

#endif  // VIGILANTE_CALLBACK_MANAGER_H_
