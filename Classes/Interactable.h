#ifndef VIGILANTE_INTERACTABLE_H_
#define VIGILANTE_INTERACTABLE_H_

namespace vigilante {

class Character;

class Interactable {
 public:
  virtual ~Interactable() = default;
  virtual void onInteract(Character* user) = 0;
};

} // namespace vigilante

#endif // VIGILANTE_INTERACTABLE_H_
