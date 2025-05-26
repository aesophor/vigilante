// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_CONTROLLABLE_H_
#define REQUIEM_CONTROLLABLE_H_

namespace requiem {

class Controllable {
 public:
  virtual ~Controllable() = default;
  virtual void handleInput() = 0;
};

}  // namespace requiem

#endif  // REQUIEM_CONTROLLABLE_H_
