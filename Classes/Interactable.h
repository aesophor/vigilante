#ifndef VIGILANTE_INTERACTABLE_H_
#define VIGILANTE_INTERACTABLE_H_

namespace vigilante {

class Interactable {
 public:
  virtual ~Interactable() = default;
  virtual void interact() = 0;
};

} // namespace vigilante

#endif // VIGILANTE_INTERACTABLE_H_
