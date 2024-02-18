// Copyright (c) 2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "InGameTime.h"

#include <algorithm>

#include "scene/GameScene.h"
#include "scene/SceneManager.h"

using namespace std;

namespace vigilante {

InGameTime::InGameTime() {
  std::make_heap(_deferredCmdsMinHeap.begin(), _deferredCmdsMinHeap.end(), cmdsMinHeapCmp);
}

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

void InGameTime::runAfter(const int hours, const int minutes, const int seconds, const string& cmd) {
  const uint64_t timestamp = _secondsElapsed + seconds + 60 * minutes + 3600 * hours;

  _deferredCmdsMinHeap.emplace_back(timestamp, cmd);
  std::push_heap(_deferredCmdsMinHeap.begin(), _deferredCmdsMinHeap.end(), cmdsMinHeapCmp);
}

void InGameTime::executeCallbacks() {
  while (_deferredCmdsMinHeap.size()) {
    const auto& [secondsElapsedRequired, cmd] = _deferredCmdsMinHeap.front();
    if (secondsElapsedRequired > _secondsElapsed) {
      return;
    }

    auto console = SceneManager::the().getCurrentScene<GameScene>()->getConsole();
    console->executeCmd(cmd);

    std::pop_heap(_deferredCmdsMinHeap.begin(), _deferredCmdsMinHeap.end(), cmdsMinHeapCmp);
    _deferredCmdsMinHeap.pop_back();
  }
}

}  // namespace vigilante
