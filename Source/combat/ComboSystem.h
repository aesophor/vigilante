// Copyright (c) 2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_COMBO_SYSTEM_H_
#define VIGILANTE_COMBO_SYSTEM_H_

#include <list>
#include <optional>
#include <unordered_map>

#include "character/Character.h"

namespace vigilante {

class Character;

template <typename V, typename E>
class Digraph final {
 public:
  using VertexId = std::size_t;
  using VertexType = V;
  using EdgeType = E;

  VertexId addVertex(V vertex) {
    const VertexId newVertexId = _vertices.size();
    _vertices.emplace_back(std::move(vertex));
    return newVertexId;
  }

  bool addOutgoingEdge(const VertexId id, E edge) {
    const std::optional<V> vertex = getVertexById(id);
    if (!vertex.has_value()) {
      return false;
    }
    _outgoingEdges[id].emplace_back(std::move(edge));
    return true;
  }

  std::optional<V> getVertexById(const VertexId id) {
    if (id >= _vertices.size()) {
      return std::nullopt;
    }
    return _vertices[id];
  }

  std::optional<std::list<E>> getOutgoingEdges(const VertexId src) const {
    const auto it = _outgoingEdges.find(src);
    if (it == _outgoingEdges.end()) {
      return std::nullopt;
    }

    return it->second;
  }

 private:
  std::vector<V> _vertices;
  std::unordered_map<VertexId, std::list<E>> _outgoingEdges;
};

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

  void update(float delta) {
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
  StateId _currentStateId{0};
  float _timer{};
};

class ComboSystem final {
  using StateTransitionRequirement = std::list<ax::EventKeyboard::KeyCode>;
  
 public:
  explicit ComboSystem(Character& c);

  void update(float delta) { _fsm.update(delta); }
  std::optional<Character::State> determineNextAttackState();

 private:
  Character& _character;
  TimedFiniteStateMachine<Character::State, StateTransitionRequirement> _fsm;
};

}  // namespace vigilante

#endif  // VIGILANTE_COMBO_SYSTEM_H_
