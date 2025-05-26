// Copyright (c) 2023-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "ComboSystem.h"

#include <algorithm>

#include "character/Character.h"
#include "input/InputManager.h"
#include "scene/SceneManager.h"
#include "util/Logger.h"

using namespace std;
USING_NS_AX;

namespace requiem {

namespace {

constexpr float kComboResetTimer = 1.0f;

}  // namespace

ComboSystem::ComboSystem(Character& c) : _character{c} {
  const auto v0 = _fsm.getInitialStateId();
  const auto v1 = _fsm.defineState(Character::State::ATTACKING);
  const auto v2 = _fsm.defineState(Character::State::ATTACKING);
  const auto v3 = _fsm.defineState(Character::State::ATTACKING_FORWARD);
  const auto v4 = _fsm.defineState(Character::State::ATTACKING_UPWARD);
  const auto v5 = _fsm.defineState(Character::State::ATTACKING_FORWARD);

  _fsm.defineStateTransition(v0, v1, {});
  _fsm.defineStateTransition(v1, v2, {});
  _fsm.defineStateTransition(v2, v4, {EventKeyboard::KeyCode::KEY_UP_ARROW});
  _fsm.defineStateTransition(v2, v3, {EventKeyboard::KeyCode::KEY_LEFT_ARROW});
  _fsm.defineStateTransition(v2, v3, {EventKeyboard::KeyCode::KEY_RIGHT_ARROW});
  _fsm.defineStateTransition(v4, v5, {EventKeyboard::KeyCode::KEY_LEFT_ARROW});
  _fsm.defineStateTransition(v4, v5, {EventKeyboard::KeyCode::KEY_RIGHT_ARROW});
}

optional<Character::State> ComboSystem::determineNextAttackState() {
  auto reqs = _fsm.getAllNextStatesAndTransitionRequirements(_fsm.getCurrentStateId());
  if (reqs.empty()) {
    reset();
    reqs = _fsm.getAllNextStatesAndTransitionRequirements(_fsm.getCurrentStateId());
  }

  if (reqs.empty()) {
    VGLOG(LOG_ERR, "Failed to perform combo state transition.");
    return std::nullopt;
  }

  auto isKeyPressed = [](const EventKeyboard::KeyCode keyCode) -> bool {
    return IS_KEY_PRESSED(keyCode);
  };
  for (const auto& [nextStateId, keyCodes] : reqs) {
    if (std::all_of(keyCodes.begin(), keyCodes.end(), isKeyPressed)) {
      _fsm.setCurrentStateId(nextStateId);
      _fsm.setTimer(kComboResetTimer);
      return _fsm.getState(nextStateId);
    }
  }

  return nullopt;
}

}  // namespace requiem
