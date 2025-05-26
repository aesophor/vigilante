// Copyright (c) 2023-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_AFTER_IMAGE_FX_MANAGER_H_
#define REQUIEM_AFTER_IMAGE_FX_MANAGER_H_

#include <unordered_map>

#include <axmol.h>

namespace requiem {

class AfterImageFxManager final {
 public:
  void update(const float delta);

  bool registerNode(const ax::Node* node,
                    const ax::Color3B& color,
                    const float durationInSec,
                    const float intervalInSec);
  bool unregisterNode(const ax::Node* node);

  static inline const ax::Color3B kPlayerAfterImageColor{55, 66, 189};

 private:
  struct AfterImageFxData final {
    ax::Color3B color;
    float durationInSec;
    float intervalInSec;
    float timerInSec{};
  };

  std::unordered_map<const ax::Node*, AfterImageFxData> _entries;
};

}  // namespace requiem

#endif  // REQUIEM_AFTER_IMAGE_FX_MANAGER_H_
