// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_COMMAND_TRIGGER_H_
#define VIGILANTE_COMMAND_TRIGGER_H_

#include "DynamicActor.h"
#include "Interactable.h"

namespace vigilante {

class CommandTrigger : public DynamicActor, public Interactable {
 public:
  CommandTrigger();
  virtual ~CommandTrigger() = default;

  virtual void showOnMap(float x, float y) override;  // DynamicActor

  virtual void onInteract(Character* user) override;  // Interactable
  virtual bool willInteractOnContact() const override;  // Interactable
  virtual void showHintUI() override;  // Interactable
  virtual void hideHintUI() override;  // Interactable

 protected:
  virtual void createHintBubbleFx() override;  // Interactable
  virtual void removeHintBubbleFx() override;  // Interactable

  void defineBody(b2BodyType bodyType,
                  float x,
                  float y,
                  short categoryBits,
                  short maskBits);
};

}  // namespace vigilante

#endif  // VIGILANTE_COMMAND_TRIGGER_H_
