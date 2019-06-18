#ifndef VIGILANTE_CHEST_H_
#define VIGILANTE_CHEST_H_

#include "Box2D/Box2D.h"

#include "DynamicActor.h"
#include "Interactable.h"

namespace vigilante {

class Chest : public DynamicActor, public Interactable {
 public:
  Chest();
  virtual ~Chest() = default;
  virtual void showOnMap(float x, float y) override; // DynamicActor

  virtual void onInteract(Character* user) override; // Interactable
  virtual bool willInteractOnContact() const override; // Interactable

 protected:
  void defineBody(b2BodyType bodyType, short categoryBits, short maskBits, float x, float y);

  bool _isOpened;
};

} // namespace vigilante

#endif // VIGILANTE_CHEST_H_
