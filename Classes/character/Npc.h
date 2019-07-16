// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_NPC_H_
#define VIGILANTE_NPC_H_

#include <string>
#include <vector>

#include "Character.h"
#include "Bot.h"
#include "Interactable.h"
#include "quest/Quest.h"

namespace vigilante {

class Npc : public Character, public Bot, public Interactable {
 public:
  struct Profile {
    Profile(const std::string& jsonFileName);
    virtual ~Profile() = default;
  };

  Npc(const std::string& jsonFileName);
  virtual ~Npc() = default;

  virtual void showOnMap(float x, float y) override; // Character
  virtual void update(float delta) override; // Character
  virtual void import(const std::string& jsonFileName) override; // Character
  virtual void receiveDamage(Character* source, int damage) override; // Character

  virtual void onInteract(Character* user) override; // Interactable
  virtual bool willInteractOnContact() const override; // Interactable

  Npc::Profile& getNpcProfile();
  const std::vector<Quest*>& getQuests() const;
  
 private:
  void defineBody(b2BodyType bodyType, short bodyCategoryBits, short bodyMaskBits,
                  short feetMaskBits, short weaponMaskBits, float x, float y) override;

  Npc::Profile _npcProfile;
  std::vector<Quest*> _quests;
};

} // namespace vigilante

#endif // VIGILANTE_NPC_H_
