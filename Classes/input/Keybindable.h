#ifndef VIGILANTE_KEYBINDABLE_H_
#define VIGILANTE_KEYBINDABLE_H_

namespace vigilante {

class Character;

class Keybindable {
 public:
  virtual ~Keybindable() = default;
  virtual bool canActivate(Character* user) = 0;
  virtual void activate(Character* user) = 0;
};

} // namespace vigilante

#endif // VIGILANTE_KEYBINDABLE_H_
