// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_DS_ALGORITHM_H_
#define VIGILANTE_DS_ALGORITHM_H_

#include <memory>
#include <vector>

namespace vigilante {

template <typename T>
std::vector<T*> uniqueVec2RawVec(const std::vector<std::unique_ptr<T>>& vec) {
  std::vector<T*> ret;
  for (const auto& p : vec) {
    ret.push_back(p.get());
  }
  return ret;
}

}  // namespace vigilante

#endif  // VIGILANTE_DS_ALGORITHM_H_
