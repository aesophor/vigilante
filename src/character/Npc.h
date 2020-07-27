// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_NPC_H_
#define VIGILANTE_NPC_H_

#include <string>
#include <vector>

#include "Character.h"
#include "Interactable.h"
#include "gameplay/DialogueTree.h"

namespace vigilante {

// Npc: Non-player character
// i.e., enemies and allies are all npcs.
class Npc : public Character, public Interactable {
 public:
  // The "disposition" of an Npc means its "friendliness" toward the player.
  enum class Disposition {
    ALLY,
    ENEMY,
    SIZE
  };

  struct Profile final {
    explicit Profile(const std::string& jsonFileName);
    ~Profile() = default;

    struct DroppedItemData {
      int chance;
      int minAmount;
      int maxAmount;
    };
    // <json, {chance, minAmount, maxAmount}>
    std::unordered_map<std::string, DroppedItemData> droppedItems;

    std::string dialogueTreeJsonFile;
    Npc::Disposition disposition;
  };


  explicit Npc(const std::string& jsonFileName);
  virtual ~Npc() = default;

  virtual void showOnMap(float x, float y) override;  // Character
  virtual void update(float delta) override;  // Character
  virtual void import(const std::string& jsonFileName) override;  // Character
  virtual void receiveDamage(Character* source, int damage) override;  // Character

  virtual void onInteract(Character* user) override;  // Interactable
  virtual bool willInteractOnContact() const override;  // Interactable
  void updateDialogueTreeIfNeeded();
  void beginDialogue();

  // TODO: maybe virtual can be disabled?
  // TODO: maybe rename moveRandomly() to beginSandbox()? idk...
  void act(float delta);  // Bot
  void moveToTarget(Character* target);  // Bot
  void moveRandomly(float delta, int minMoveDuration, int maxMoveDuration,
                            int minWaitDuration, int maxWaitDuration);  // Bot
  void jumpIfStucked(float delta, float checkInterval);  // Bot
  void reverseDirection();  // Bot

  Npc::Profile& getNpcProfile();
  DialogueTree& getDialogueTree();
  Npc::Disposition getDisposition() const;
  void setDisposition(Npc::Disposition disposition);


 private:
  void defineBody(b2BodyType bodyType, float x, float y,
                  short bodyCategoryBits=0, short bodyMaskBits=0,
                  short feetMaskBits=0, short weaponMaskBits=0) override;


  Npc::Profile _npcProfile;
  DialogueTree _dialogueTree;
  Npc::Disposition _disposition;

  // The following variables are used in Npc::moveRandomly()
  bool _isMovingRight;
  float _moveDuration;
  float _moveTimer;
  float _waitDuration;
  float _waitTimer;

  // The following variables are used in Npc::jumpIfStucked()
  b2Vec2 _lastStoppedPosition;
  float _lastTraveledDistance;
  float _calculateDistanceTimer;
};

}  // namespace vigilante

#endif  // VIGILANTE_NPC_H_
