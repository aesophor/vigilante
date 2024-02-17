// Copyright (c) 2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "InGameTime.h"

#include "util/Logger.h"

using namespace std;

namespace vigilante {

void InGameTime::update(const float delta) {
  updateTime(delta);
  executeCallbacks();
}

void InGameTime::updateTime(const float delta) {
  _accumulatedDelta += delta;
  if (_accumulatedDelta < _updateDeltaThreshold) {
    return;
  }

  const int secondDelta = _accumulatedDelta * kTimeScale;
  _accumulatedDelta = 0.0f;
  _secondsElapsed += secondDelta;

  _second += secondDelta;
  while (_second > 59) {
    _minute++;
    _second -= 60;
  }
  while (_minute > 59) {
    _hour++;
    _minute -= 60;
  }
  while (_hour > 23) {
    _hour -= 24;
  }
}

void InGameTime::runAfter(const int hours, const int minutes, const int seconds, function<void ()>&& callback) {
  const uint64_t timestamp = _secondsElapsed + seconds + 60 * minutes + 3600 * hours;
  _callbackPriorityQueue.emplace(timestamp, std::move(callback));
}

void InGameTime::executeCallbacks() {
  while (_callbackPriorityQueue.size()) {
    const auto& [timestamp, callback] = _callbackPriorityQueue.top();
    if (timestamp > _secondsElapsed) {
      return;
    }

    callback();
    _callbackPriorityQueue.pop();
  }
}

}  // namespace vigilante
