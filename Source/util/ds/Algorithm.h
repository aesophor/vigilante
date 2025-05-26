// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_UTIL_DS_ALGORITHM_H_
#define REQUIEM_UTIL_DS_ALGORITHM_H_

#include <memory>
#include <vector>

namespace requiem {

template <typename T>
std::vector<T*> uniqueVec2RawVec(const std::vector<std::unique_ptr<T>>& vec) {
  std::vector<T*> ret;
  for (const auto& p : vec) {
    ret.push_back(p.get());
  }
  return ret;
}

}  // namespace requiem

#endif  // REQUIEM_UTIL_DS_ALGORITHM_H_
