// Copyright (c) 2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_GAMEPLAY_IN_GAME_TIME_H_
#define VIGILANTE_GAMEPLAY_IN_GAME_TIME_H_

#include <functional>
#include <queue>
#include <vector>

namespace vigilante {

class InGameTime final {
 public:
  void update(const float delta);
  void runAfter(const int hours, const int minutes, const int seconds, std::function<void ()>&& callback);

  inline int getHour() const { return _hour; }
  inline int getMinute() const { return _minute; }
  inline int getSecond() const { return _second; }

  static constexpr int kTimeScale = 20;

 private:
  void updateTime(const float delta);
  void executeCallbacks();

  float _updateDeltaThreshold{1.0f / kTimeScale};
  float _accumulatedDelta{};

  // In-game time.
  int _hour{};
  int _minute{};
  int _second{};

  // The number of in-game seconds elapsed since the beginning of the game (per-save).
  uint64_t _secondsElapsed{};

  using T = std::pair<uint64_t, std::function<void ()>>;
  static constexpr auto cmp = [](const T& a, const T& b) -> bool { return a.first < b.first; };
  std::priority_queue<T, std::vector<T>, decltype(cmp)> _callbackPriorityQueue;
};

}  // namespace vigilante

#endif  // VIGILANTE_GAMEPLAY_IN_GAME_TIME_H_
