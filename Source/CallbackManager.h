// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_CALLBACK_MANAGER_H_
#define REQUIEM_CALLBACK_MANAGER_H_

#include <cstdint>
#include <functional>
#include <unordered_set>

#include <axmol.h>

namespace requiem {

class CallbackManager final {
 public:
  using CallbackId = uint64_t;
  static CallbackManager& the();

  CallbackId runAfter(std::function<void (const CallbackId id)>&& userCallback, float delay);
  void cancel(const CallbackId id);

  inline void setScene(ax::Scene* scene) { _scene = scene; }

 private:
  CallbackManager() : _scene() {}

  static inline CallbackId _nextCallbackId{1};

  ax::Scene* _scene;
  std::unordered_set<CallbackId> _cancelledCallbackIds;
};

}  // namespace requiem

#endif  // REQUIEM_CALLBACK_MANAGER_H_
