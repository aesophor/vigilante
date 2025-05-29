// Copyright (c) 2025 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "TimeUtil.h"

#include <chrono>

using namespace std;

namespace requiem::time_util {

uint64_t getCurrentTimeMs() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

}  // namespace requiem::time_util
