// Copyright (c) 2024-2025 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_GAMEPLAY_IN_GAME_TIME_H_
#define VIGILANTE_GAMEPLAY_IN_GAME_TIME_H_

#include <string>
#include <vector>

namespace vigilante {

class InGameTime final {
 public:
  using DeferredCmd = std::pair<uint64_t, std::string>;

  InGameTime();

  void update(const float delta);
  void fastForward(const int secondsDelta);
  void runAfter(const int hours, const int minutes, const int seconds, const std::string& cmd);

  inline bool isDawn() const { return _hour >= 5 && _hour <= 6; }
  inline bool isDay() const { return _hour > 6 && _hour < 18; }
  inline bool isDusk() const { return _hour >= 18 && _hour <= 19; }
  inline bool isNight() const { return _hour > 19 || _hour < 5; }

  inline int getHour() const { return _hour; }
  inline int getMinute() const { return _minute; }
  inline int getSecond() const { return _second; }
  inline uint64_t getSecondsElapsed() const { return _secondsElapsed; }
  inline const std::vector<DeferredCmd>& getDeferredCmdsMinHeap() const { return _deferredCmdsMinHeap; }

  inline void setHour(const int hour) { _hour = hour; }
  inline void setMinute(const int minute) { _minute = minute; }
  inline void setSecond(const int second) { _second = second; }
  inline void setSecondsElapsed(const uint64_t secondsElapsed) { _secondsElapsed = secondsElapsed; }
  inline void setdDeferredCmdsMinHeap(std::vector<DeferredCmd>&& deferredCmdsMinHeap) {
    _deferredCmdsMinHeap = std::move(deferredCmdsMinHeap);
  }

  static constexpr int kTimeScale = 20;

  // Dawn: [05:00, 06:59]
  static constexpr int kDawnBeginHour = 5;
  static constexpr int kDawnEndHour = 7;

  // Dusk: [18:00, 19:59]
  static constexpr int kDuskBeginHour = 18;
  static constexpr int kDuskEndHour = 20;

 private:
  void updateTime(const float delta);
  void executeCallbacks();

  static constexpr bool cmdsMinHeapCmp(const DeferredCmd& p1, const DeferredCmd& p2) {
    return p1.first > p2.first;
  }

  float _updateDeltaThreshold{1.0f / kTimeScale};
  float _accumulatedDelta{};

  // In-game time.
  int _hour{8};
  int _minute{30};
  int _second{};

  // The number of in-game seconds elapsed since the beginning of the game (per-save).
  uint64_t _secondsElapsed{};

  // The vector of cmds to be executed after a specific value of `_secondsElapsed`.
  std::vector<DeferredCmd> _deferredCmdsMinHeap;
};

}  // namespace vigilante

#endif  // VIGILANTE_GAMEPLAY_IN_GAME_TIME_H_
