// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_PROJECTILE_H_
#define VIGILANTE_PROJECTILE_H_

namespace vigilante {

class Character;

// Projectile interface
class Projectile {
 public:
  virtual ~Projectile() = default;

  virtual Character* getUser() const = 0;
  virtual int getDamage() const = 0;
  virtual void onHit(Character* target) = 0;
};

}  // namespace vigilante

#endif  // VIGILANTE_PROJECTILE_H_
