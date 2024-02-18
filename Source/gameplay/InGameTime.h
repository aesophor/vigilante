// Copyright (c) 2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

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
  void runAfter(const int hours, const int minutes, const int seconds, const std::string& cmd);

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

 private:
  void updateTime(const float delta);
  void executeCallbacks();

  static constexpr bool cmdsMinHeapCmp(const DeferredCmd& p1, const DeferredCmd& p2) {
    return p1.first > p2.first;
  }

  float _updateDeltaThreshold{1.0f / kTimeScale};
  float _accumulatedDelta{};

  // In-game time.
  int _hour{};
  int _minute{};
  int _second{};

  // The number of in-game seconds elapsed since the beginning of the game (per-save).
  uint64_t _secondsElapsed{};

  // The vector of cmds to be executed after a specific value of `_secondsElapsed`.
  std::vector<DeferredCmd> _deferredCmdsMinHeap;
};

}  // namespace vigilante

#endif  // VIGILANTE_GAMEPLAY_IN_GAME_TIME_H_
