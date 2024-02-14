// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_UI_TIMED_LABEL_SERVICE_H_
#define VIGILANTE_UI_TIMED_LABEL_SERVICE_H_

#include <string>

#include <axmol.h>
#include <2d/Label.h>

namespace vigilante {

class TimedLabelService {
 public:
  struct TimedLabel {
    using Alignment = ax::Vec2;
    static inline Alignment kLeft = {0, 1};
    static inline Alignment kCenter = {0.5, 1};
    static inline Alignment kRight = {1, 1};

    TimedLabel(const std::string& text, const float lifetime, const TimedLabel::Alignment alignment);

    bool operator== (const TimedLabel& other) {
      return label == other.label;
    }

    ax::Label* label;
    float lifetime;
    float timer{};
  };

  virtual ~TimedLabelService() = default;

  void update(const float delta);
  void show(const std::string& message);
  inline ax::Layer* getLayer() const { return _layer; }

 protected:
  TimedLabelService(const float startingX, const float startingY,
                    const uint8_t maxLabelCount, const uint8_t labelLifetime,
                    const TimedLabelService::TimedLabel::Alignment alignment);

  static inline constexpr float _kMoveUpDuration = .2f;
  static inline constexpr float _kFadeDuration = 1.0f;
  static inline constexpr float _kDeltaX = 0.0f;
  static inline constexpr float _kDeltaY = 13.0f;
 
  ax::Layer* _layer;
  std::deque<TimedLabelService::TimedLabel> _labelQueue;

  const float _kStartingX;
  const float _kStartingY;
  const uint8_t _kMaxLabelCount;
  const uint8_t _kLabelLifetime;
  const TimedLabelService::TimedLabel::Alignment _kAlignment;
};

}  // namespace vigilante

#endif  // VIGILANTE_UI_TIMED_LABEL_SERVICE_H_
