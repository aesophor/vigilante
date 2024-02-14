// Copyright (c) 2023-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_COMBAT_COMBO_SYSTEM_H_
#define VIGILANTE_COMBAT_COMBO_SYSTEM_H_

#include "character/Character.h"
#include "util/ds/Digraph.h"

namespace vigilante {

class Character;

template <typename S, typename T>
class TimedFiniteStateMachine final {
 public:
  using StateId = std::size_t;
  using DigraphType = Digraph<S, std::pair<StateId, T>>;

  TimedFiniteStateMachine() {
    _digraph.addVertex(_initialState);
  }

  StateId defineState(const S state) {
    return _digraph.addVertex(state);
  }

  bool defineStateTransition(const StateId src, const StateId dst, T transitionRequirement) {
    return _digraph.addOutgoingEdge(src, std::make_pair(dst, std::move(transitionRequirement)));
  }

  std::optional<S> getState(const StateId id) {
    return _digraph.getVertexById(id);
  }

  std::list<typename DigraphType::EdgeType>
  getAllNextStatesAndTransitionRequirements(const StateId sourceStateId) const {
    const auto ret = _digraph.getOutgoingEdges(sourceStateId);
    if (!ret.has_value()) {
      return {};
    }
    return *ret;
  }

  void update(const float delta) {
    if (_timer <= 0.0f) {
      return;
    }

    _timer -= delta;
    if (_timer <= 0.0f) {
      resetCurrentStateId();
    }
  }

  StateId getInitialStateId() const { return 0; }
  StateId getCurrentStateId() const { return _currentStateId; }
  void setCurrentStateId(const StateId id) { _currentStateId = id; }
  void resetCurrentStateId() { _currentStateId = {}; }
  void setTimer(const float timer) { _timer = timer; }

 private:
  DigraphType _digraph;
  const S _initialState{};
  StateId _currentStateId{};
  float _timer{};
};

class ComboSystem final {
  using StateTransitionRequirement = std::list<ax::EventKeyboard::KeyCode>;

 public:
  explicit ComboSystem(Character& c);

  void update(const float delta) {
    _fsm.update(delta);
    if (_fsm.getCurrentStateId() == 0) {
      _character.resetAttackAnimationIdx();
    }
  }
  
  void reset() {
    _fsm.resetCurrentStateId();
    _fsm.setTimer(0.0f);
  }

  std::optional<Character::State> determineNextAttackState();

 private:
  Character& _character;
  TimedFiniteStateMachine<Character::State, StateTransitionRequirement> _fsm;
};

}  // namespace vigilante

#endif  // VIGILANTE_COMBAT_COMBO_SYSTEM_H_
