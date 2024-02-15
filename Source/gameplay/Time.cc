// Copyright (c) 2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "Time.h"

#include "util/Logger.h"

namespace vigilante {

void Time::update(const float delta) {
  _accumulatedDelta += delta;
  if (_accumulatedDelta < _updateDeltaThreshold) {
    return;
  }

  const int secondDelta = _accumulatedDelta * _timeScale;
  _accumulatedDelta = 0.0f;

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

}  // namespace vigilante
