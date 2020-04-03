// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_TIMED_LABEL_SERVICE_H_
#define VIGILANTE_TIMED_LABEL_SERVICE_H_

#include <string>

#include <2d/CCLabel.h>
#include <cocos2d.h>

namespace vigilante {

class TimedLabelService {
 public:
  struct TimedLabel {
    using Alignment = cocos2d::Vec2;
    static const Alignment kLeft;
    static const Alignment kCenter;
    static const Alignment kRight;

    TimedLabel(const std::string& text, float lifetime, TimedLabel::Alignment alignment);
    bool operator==(const TimedLabel& other);

    cocos2d::Label* label;
    float lifetime;
    float timer;
  };

  virtual ~TimedLabelService() = default;

  void update(float delta);
  void show(const std::string& message);
  cocos2d::Layer* getLayer() const;

 protected:
  TimedLabelService(int startingX, int startingY, uint8_t maxLabelCount, uint8_t labelLifetime,
                    TimedLabelService::TimedLabel::Alignment alignment);

  static const float _kMoveUpDuration;
  static const float _kFadeDuration;
  static const float _kDeltaX;
  static const float _kDeltaY;

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
