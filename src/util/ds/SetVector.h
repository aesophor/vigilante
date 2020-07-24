// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_SET_VECTOR_H_
#define VIGILANTE_SET_VECTOR_H_

#include <unordered_set>
#include <vector>

namespace vigilante {

// A vector that contains a set of unique objects of type Key
// where the order of iteration is the order of insertion.

template <typename Key>
class SetVector {
 public:
  using size_type = typename std::vector<Key>::size_type;
  using iterator = typename std::vector<Key>::iterator;
  using const_iterator = typename std::vector<Key>::const_iterator;

  SetVector() : _set(), _vec() {}
  
  SetVector(const SetVector& other)
      : _set(other._set), _vec(other._vec) {}

  SetVector(SetVector&& other) noexcept
      : _set(std::move(other._set)), _vec(std::move(other._vec)) {}

  SetVector& operator=(const SetVector& other) {
    _set = other._set;
    _vec = other._vec;
    return *this;
  }
  
  SetVector& operator=(SetVector&& other) noexcept {
    _set = std::move(other._set);
    _vec = std::move(other._vec);
    return *this;
  }
  
  virtual ~SetVector() = default;


  void insert(Key key) {
    auto it = _set.find(key);
    if (it != _set.end()) {
      return;
    }
    _vec.push_back(key);
    _set.insert(key);
  }

  size_type erase(Key key) {
    auto it = _set.find(key);
    if (it == _set.end()) {
      return 0;
    }

    _vec.erase(std::remove(_vec.begin(), _vec.end(), key), _vec.end());
    _set.erase(key);
    return 1;
  }

  void clear() {
    _set.clear();
    _vec.clear();
  }
  

  bool empty() const {
    return _vec.empty();
  }

  size_type size() const {
    return _vec.size();
  }

  iterator begin() {
    return _vec.begin();
  }

  iterator end() {
    return _vec.end();
  }

  const_iterator begin() const {
    return _vec.begin();
  }

  const_iterator end() const {
    return _vec.end();
  }


  Key& front() {
    return _vec.front();
  }

  Key& back() {
    return _vec.back();
  }

 protected:
  std::unordered_set<Key> _set;
  std::vector<Key> _vec;
};

}  // namespace vigilante

#endif  // VIGILANTE_SET_VECTOR_H_
