// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_INTERACTABLE_H_
#define REQUIEM_INTERACTABLE_H_

namespace requiem {

class Character;

class Interactable {
 public:
  virtual ~Interactable() = default;

  virtual void onInteract(Character* user) = 0;
  virtual bool willInteractOnContact() const = 0;
  virtual void showHintUI() = 0;
  virtual void hideHintUI() = 0;

 protected:
  virtual void createHintBubbleFx() = 0;
  virtual void removeHintBubbleFx() = 0;
};

}  // namespace requiem

#endif  // REQUIEM_INTERACTABLE_H_
