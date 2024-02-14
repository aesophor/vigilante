// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_CONTROLLABLE_H_
#define VIGILANTE_CONTROLLABLE_H_

namespace vigilante {

class Controllable {
 public:
  virtual ~Controllable() = default;
  virtual void handleInput() = 0;
};

}  // namespace vigilante

#endif  // VIGILANTE_CONTROLLABLE_H_
