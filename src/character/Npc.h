// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_NPC_H_
#define VIGILANTE_NPC_H_

#include <string>
#include <vector>

#include "Character.h"
#include "Interactable.h"
#include "gameplay/DialogueTree.h"

namespace vigilante {

class Npc : public Character, public Interactable {
 public:
  struct Profile {
    explicit Profile(const std::string& jsonFileName);

    std::string dialogueTree;
  };

  explicit Npc(const std::string& jsonFileName);
  virtual ~Npc() = default;

  virtual void showOnMap(float x, float y) override; // Character
  virtual void update(float delta) override; // Character
  virtual void import(const std::string& jsonFileName) override; // Character
  virtual void receiveDamage(Character* source, int damage) override; // Character

  virtual void onInteract(Character* user) override; // Interactable
  virtual bool willInteractOnContact() const override; // Interactable

  Npc::Profile& getNpcProfile();
  DialogueTree& getDialogueTree();
  
 private:
  void defineBody(b2BodyType bodyType, short bodyCategoryBits, short bodyMaskBits,
                  short feetMaskBits, short weaponMaskBits, float x, float y) override;

  Npc::Profile _npcProfile;
  DialogueTree _dialogueTree;
};

} // namespace vigilante

#endif // VIGILANTE_NPC_H_
