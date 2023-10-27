// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_NPC_H_
#define VIGILANTE_NPC_H_

#include <string>
#include <unordered_map>

#include <axmol.h>

#include "Interactable.h"
#include "character/Character.h"
#include "character/NpcController.h"
#include "gameplay/DialogueTree.h"
#include "ui/hud/StatusBar.h"

namespace vigilante {

// Npc: Non-player character
// i.e., enemies and allies are all npcs.
class Npc : public Character, public Interactable {
 public:
  // In addition to Character::FixtureType, the new version defined in Npc.h
  // has the fourth fixture type.
  enum FixtureType {
    BODY,
    FEET,
    WEAPON,
    INTERACTABLE,  // used in player's interaction with Npcs (with FEET fixture)
    FIXTURE_SIZE
  };

  // The "disposition" of an Npc means its "friendliness" toward the player.
  enum class Disposition {
    ALLY,
    ENEMY,
    SIZE
  };

  struct Profile final {
    explicit Profile(const std::string& jsonFileName);

    struct DroppedItemData {
      int chance;
      int minAmount;
      int maxAmount;
    };
    // <json, {chance, minAmount, maxAmount}>
    std::unordered_map<std::string, DroppedItemData> droppedItems;

    std::string dialogueTreeJsonFile;
    Npc::Disposition disposition;
    bool isRespawnable;
    bool isRecruitable;
    bool isTradable;
    bool shouldSandbox;
  };

  explicit Npc(const std::string& jsonFileName);
  virtual ~Npc() override = default;

  virtual bool showOnMap(float x, float y) override;  // Character
  virtual bool removeFromMap() override;  // Character
  virtual void update(const float delta) override;  // Character
  virtual void import(const std::string& jsonFileName) override;  // Character

  virtual void onKilled() override;  // Character
  virtual void onMapChanged() override;  // Character

  virtual bool inflictDamage(Character* target, int damage) override;  // Character
  virtual bool receiveDamage(Character* source, int damage) override;  // Character
  virtual void interact(Interactable* target) override;  // Character

  virtual void onInteract(Character* user) override;  // Interactable
  virtual bool willInteractOnContact() const override;  // Interactable
  virtual void showHintUI() override;  // Interactable
  virtual void hideHintUI() override;  // Interactable

  void act(const float delta) { _npcController.update(delta); }
  void reverseDirection() { _npcController.reverseDirection(); }
  void dropItems();

  void updateDialogueTreeIfNeeded();
  void onDialogueBegin();
  void onDialogueEnd();
  void beginDialogue();
  void beginTrade();

  bool isPlayerLeaderOfParty() const;
  bool isWaitingForPartyLeader() const;
  bool isWaitingForPlayer() const;

  inline Npc::Profile& getNpcProfile() { return _npcProfile; }
  inline DialogueTree& getDialogueTree() { return _dialogueTree; }
  inline Npc::Disposition getDisposition() const { return _disposition; }
  void setDisposition(Npc::Disposition disposition);

  inline bool isSandboxing() const { return _npcController.isSandboxing(); }
  inline void setSandboxing(const bool sandboxing) { _npcController.setSandboxing(sandboxing); }

 private:
  virtual void defineBody(b2BodyType bodyType,
                          float x,
                          float y,
                          short bodyCategoryBits=0,
                          short bodyMaskBits=0,
                          short feetMaskBits=0,
                          short weaponMaskBits=0) override;  // Character

  virtual void createHintBubbleFx() override;  // Interactable
  virtual void removeHintBubbleFx() override;  // Interactable

  Npc::Profile _npcProfile;
  DialogueTree _dialogueTree;
  Npc::Disposition _disposition;
  NpcController _npcController;

  ax::Sprite* _hintBubbleFxSprite{};
  std::unique_ptr<StatusBar> _floatingHealthBar;
};

}  // namespace vigilante

#endif  // VIGILANTE_NPC_H_
