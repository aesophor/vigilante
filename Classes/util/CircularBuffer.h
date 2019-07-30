// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_CIRCULAR_BUFFER_H_
#define VIGILANTE_CIRCULAR_BUFFER_H_

#include <string>
#include <memory>

#define DEFAULT_CAPACITY 32

namespace vigilante {

template <typename T>
class CircularBuffer {
 public:
  CircularBuffer(int capacity=DEFAULT_CAPACITY);
  virtual ~CircularBuffer() = default;
  T& operator[] (size_t i);

  void push(T val);
  void pop();
  void clear();

  size_t size() const;
  size_t capacity() const;
  bool empty() const;
  bool full() const;
  T front() const;
  T back() const;

 protected:
  std::unique_ptr<T[]> _data;
  int _head;
  int _tail;
  size_t _size;
  const size_t _capacity;
};



template <typename T>
CircularBuffer<T>::CircularBuffer(int capacity)
    : _data(new T[capacity]), _head(), _tail(), _size(), _capacity(capacity) {}

template <typename T>
T& CircularBuffer<T>::operator[] (size_t i) {
  return _data[i];
}


template <typename T>
void CircularBuffer<T>::push(T val) {
  _data[_tail] = val;
  
  if (full()) {
    _head = (_head + 1) % _capacity;
  }
  _tail = (_tail + 1) % _capacity;

  if (_size < _capacity) {
    _size++;
  }
}

template <typename T>
void CircularBuffer<T>::pop() {
  _head = (_head + 1) % _capacity;

  if (_size > 0) {
    _size--;
  }
}

template <typename T>
void CircularBuffer<T>::clear() {
  _tail = 0;
  _head = 0;
}


template <typename T>
size_t CircularBuffer<T>::size() const {
  return _size;
}

template <typename T>
size_t CircularBuffer<T>::capacity() const {
  return _capacity;
}

template <typename T>
bool CircularBuffer<T>::empty() const {
  return _tail == _head;
}

template <typename T>
bool CircularBuffer<T>::full() const {
  return (_tail + 1) % (int) _capacity == _head;
}

template <typename T>
T CircularBuffer<T>::front() const {
  return _data[_head];
}

template <typename T>
T CircularBuffer<T>::back() const {
  return _data[(_tail - 1 < 0) ? _capacity - 1 : _tail - 1];
}

} // namespace vigilante

#endif // VIGILANTE_CIRCULAR_BUFFER_H_
