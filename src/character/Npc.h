// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_NPC_H_
#define VIGILANTE_NPC_H_

#include <atomic>
#include <string>
#include <unordered_set>
#include <vector>

#include <cocos2d.h>

#include "Interactable.h"
#include "character/Character.h"
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
    bool isRespawnable;
    bool isRecruitable;
    bool isUnsheathed;
    bool isTradable;
    bool shouldSandbox;
  };

  // In addition to Character::FixtureType, the new version defined in Npc.h
  // has the fourth fixture type.
  enum FixtureType {
    BODY,
    FEET,
    WEAPON,
    INTERACTABLE,  // used in player's interaction with Npcs (with FEET fixture)
    FIXTURE_SIZE
  };

  explicit Npc(const std::string& jsonFileName);
  virtual ~Npc() = default;

  virtual bool showOnMap(float x, float y) override;  // Character
  virtual void update(float delta) override;  // Character
  virtual void import(const std::string& jsonFileName) override;  // Character

  virtual void onKilled() override;  // Character
  virtual void onMapChanged() override;  // Character

  virtual void inflictDamage(Character* target, int damage) override;  // Character
  virtual void receiveDamage(Character* source, int damage) override;  // Character
  virtual void interact(Interactable* target) override;  // Character

  virtual void onInteract(Character* user) override;  // Interactable
  virtual bool willInteractOnContact() const override;  // Interactable
  virtual void showHintUI() override;  // Interactable
  virtual void hideHintUI() override;  // Interactable

  void dropItems();

  void updateDialogueTreeIfNeeded();
  void beginDialogue();
  void beginTrade();

  void onDialogueBegin();
  void onDialogueEnd();

  void act(float delta);
  void findNewLockedOnTargetFromParty(Character* killedTarget);
  bool isTooFarAwayFromTarget(Character* target) const;
  void teleportToTarget(Character* target);
  void teleportToTarget(const b2Vec2& targetPos);
  void moveToTarget(float delta, Character* target, float followDist);
  void moveToTarget(float delta, const b2Vec2& targetPos, float followDist);
  void moveRandomly(float delta,
                    int minMoveDuration, int maxMoveDuration,
                    int minWaitDuration, int maxWaitDuration);
  void jumpIfStucked(float delta, float checkInterval);
  void reverseDirection();

  bool isPlayerLeaderOfParty() const;
  bool isWaitingForPartyLeader() const;
  bool isWaitingForPlayer() const;

  inline Npc::Profile& getNpcProfile() { return _npcProfile; }
  inline DialogueTree& getDialogueTree() { return _dialogueTree; }
  inline Npc::Disposition getDisposition() const { return _disposition; }
  void setDisposition(Npc::Disposition disposition);

  inline bool isSandboxing() const { return _isSandboxing; }
  inline void setSandboxing(bool sandboxing) { _isSandboxing = sandboxing; }

  inline void clearMoveDest() { _moveDest.SetZero(); }

  static inline void setNpcsAllowedToAct(bool npcsAllowedToAct) {
    Npc::_areNpcsAllowedToAct = npcsAllowedToAct;
  }

  static bool isNpcAllowedToSpawn(const std::string& jsonFileName);
  static void setNpcAllowedToSpawn(const std::string& jsonFileName, bool canSpawn);

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

  static inline constexpr float _kAllyTeleportDist = 2.5f;
  static inline constexpr float _kAllyFollowDist = .75f;
  static inline constexpr float _kMoveDestFollowDist = .2f;
  static inline constexpr float _kJumpCheckInterval = .5f;

  // See `map/GameMap.cc` for its usage.
  static inline std::atomic<bool> _areNpcsAllowedToAct{true};

  // Once those spawn-once NPCs are killed, their jsonFileName
  // will be inserted into this unordered_set.
  static inline std::unordered_set<std::string> _npcSpawningBlacklist;

  Npc::Profile _npcProfile;
  DialogueTree _dialogueTree;
  Npc::Disposition _disposition;
  bool _isSandboxing;

  cocos2d::Sprite* _hintBubbleFxSprite;

  b2Vec2 _moveDest;

  bool _isMovingRight;
  float _moveDuration;
  float _moveTimer;
  float _waitDuration;
  float _waitTimer;

  float _calculateDistanceTimer;
  b2Vec2 _lastStoppedPosition;
};

}  // namespace vigilante

#endif  // VIGILANTE_NPC_H_
