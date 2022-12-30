// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_TIMED_LABEL_SERVICE_H_
#define VIGILANTE_TIMED_LABEL_SERVICE_H_

#include <string>

#include <cocos2d.h>
#include <2d/CCLabel.h>

namespace vigilante {

class TimedLabelService {
 public:
  struct TimedLabel {
    using Alignment = cocos2d::Vec2;
    static inline Alignment kLeft = {0, 1};
    static inline Alignment kCenter = {0.5, 1};
    static inline Alignment kRight = {1, 1};

    TimedLabel(const std::string& text, float lifetime,
               TimedLabel::Alignment alignment);

    bool operator== (const TimedLabel& other) {
      return label == other.label;
    }

    cocos2d::Label* label;
    float lifetime;
    float timer;
  };

  virtual ~TimedLabelService() = default;

  void update(float delta);
  void show(const std::string& message);
  inline cocos2d::Layer* getLayer() const { return _layer; }

 protected:
  TimedLabelService(int startingX, int startingY,
                    uint8_t maxLabelCount, uint8_t labelLifetime,
                    TimedLabelService::TimedLabel::Alignment alignment);

  static inline constexpr float _kMoveUpDuration = .2f;
  static inline constexpr float _kFadeDuration = 1.0f;
  static inline constexpr float _kDeltaX = 0.0f;
  static inline constexpr float _kDeltaY = 13.0f;
 
  cocos2d::Layer* _layer;
  std::deque<TimedLabelService::TimedLabel> _labelQueue;

  const float _kStartingX;
  const float _kStartingY;
  const uint8_t _kMaxLabelCount;
  const uint8_t _kLabelLifetime;
  const TimedLabelService::TimedLabel::Alignment _kAlignment;
};

}  // namespace vigilante

#endif  // VIGILANTE_TIMED_LABEL_SERVICE_H_
