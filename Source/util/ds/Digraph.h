// Copyright (c) 2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_DIGRAPH_H_
#define VIGILANTE_DIGRAPH_H_

#include <list>
#include <optional>
#include <unordered_map>
#include <vector>

namespace vigilante {

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

}  // namespace vigilante

#endif  // VIGILANTE_DIGRAPH_H_
