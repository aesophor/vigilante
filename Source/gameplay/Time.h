// Copyright (c) 2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_GAMEPLAY_TIME_H_
#define VIGILANTE_GAMEPLAY_TIME_H_

namespace vigilante {

class Time final {
 public:
  Time() = default;

  void update(const float delta);

  inline int getHour() const { return _hour; }
  inline int getMinute() const { return _minute; }
  inline int getSecond() const { return _second; }
  inline void setTimeScale(const int timeScale) {
    _timeScale = timeScale;
    _updateDeltaThreshold = 1.0f / timeScale;
  }

 private:
  int _timeScale{20};
  float _updateDeltaThreshold{1.0f / 20};
  float _accumulatedDelta{};

  int _hour{};
  int _minute{};
  int _second{};
};

}  // namespace vigilante

#endif  // VIGILANTE_GAMEPLAY_TIME_H_
