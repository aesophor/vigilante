#ifndef VIGILANTE_CHEST_H_
#define VIGILANTE_CHEST_H_

#include <vector>

#include "Box2D/Box2D.h"

#include "DynamicActor.h"
#include "Interactable.h"
#include "item/Item.h"

namespace vigilante {

class Chest : public Interactable, public DynamicActor {
 public:
  Chest();
  virtual ~Chest();
  virtual void showOnMap(float x, float y) override; // DynamicActor

  virtual void onInteract(Character* user) override; // Interactable
  virtual bool willInteractOnContact() const override; // Interactable

  std::vector<Item*>& getItems();

 protected:
  void defineBody(b2BodyType bodyType, short categoryBits, short maskBits, float x, float y);

  static const int _kNumAnimations;
  static const int _kNumFixtures;

  std::vector<Item*> _items;
  bool _isOpened;
};

} // namespace vigilante

#endif // VIGILANTE_CHEST_H_
