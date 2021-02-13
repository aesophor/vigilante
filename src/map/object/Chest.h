// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_CHEST_H_
#define VIGILANTE_CHEST_H_

#include <string>
#include <vector>

#include <cocos2d.h>
#include <Box2D/Box2D.h>
#include "DynamicActor.h"
#include "Interactable.h"

namespace vigilante {

class Chest : public DynamicActor, public Interactable {
 public:
  Chest();
  explicit Chest(const std::string& itemsJson);
  virtual ~Chest() = default;
  virtual void showOnMap(float x, float y) override;  // DynamicActor

  virtual void onInteract(Character* user) override;  // Interactable
  virtual bool willInteractOnContact() const override;  // Interactable
  virtual void showHintUI() override;  // Interactable
  virtual void hideHintUI() override;  // Interactable

 protected:
  virtual void createHintBubbleFx() override;  // Interactable
  virtual void removeHintBubbleFx() override;  // Interactable

  void defineBody(b2BodyType bodyType,
                  short categoryBits,
                  short maskBits,
                  float x,
                  float y);

  static const int _kNumAnimations;
  static const int _kNumFixtures;

  cocos2d::Sprite* _hintBubbleFxSprite;

  std::vector<std::string> _itemJsons;
  bool _isOpened;
};

}  // namespace vigilante

#endif  // VIGILANTE_CHEST_H_
