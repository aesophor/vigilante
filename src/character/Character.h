// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_CHARACTER_H_
#define VIGILANTE_CHARACTER_H_

#include <array>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <Box2D/Box2D.h>
#include <cocos2d.h>

#include "DynamicActor.h"
#include "Importable.h"
#include "Interactable.h"
#include "character/Party.h"
#include "item/Item.h"
#include "item/Equipment.h"
#include "item/Consumable.h"
#include "map/GameMap.h"
#include "skill/Skill.h"
#include "util/ds/SetVector.h"

namespace vigilante {

class Character : public DynamicActor, public Importable {
 protected:
  enum State {
    IDLE_SHEATHED,
    IDLE_UNSHEATHED,
    RUNNING_SHEATHED,
    RUNNING_UNSHEATHED,
    JUMPING_SHEATHED,
    JUMPING_UNSHEATHED,
    FALLING_SHEATHED,
    FALLING_UNSHEATHED,
    CROUCHING_SHEATHED,
    CROUCHING_UNSHEATHED,
    SHEATHING_WEAPON,
    UNSHEATHING_WEAPON,
    ATTACKING,
    KILLED,
    STATE_SIZE,
    FORCE_UPDATE
  };

  enum Sfx {
    SFX_JUMP,
    SFX_HURT,
    SFX_KILLED,
    SFX_SIZE,
  };

 public:
  using Inventory = std::array<SetVector<Item*>, Item::Type::SIZE>;
  using EquipmentSlots = std::array<Equipment*, Equipment::Type::SIZE>;
  using SkillBook = std::array<SetVector<Skill*>, Skill::Type::SIZE>;

  struct Profile final {
    explicit Profile(const std::string& jsonFileName);

    std::string jsonFileName;
    std::string textureResDir;
    float spriteOffsetX;
    float spriteOffsetY;
    float spriteScaleX;
    float spriteScaleY;
    std::array<float, Character::State::STATE_SIZE> frameIntervals;
    std::array<std::string, Character::Sfx::SFX_SIZE> sfxFileNames;

    std::string name;
    int level;
    int exp;

    int fullHealth;
    int fullStamina;
    int fullMagicka;
    int health;
    int stamina;
    int magicka;

    int strength;
    int dexterity;
    int intelligence;
    int luck;

    int bodyWidth;
    int bodyHeight;
    float moveSpeed;
    float jumpHeight;
    bool canDoubleJump;

    float attackForce;
    float attackTime;
    float attackRange;
    int baseMeleeDamage;

    std::vector<std::string> defaultSkills;
    std::vector<std::pair<std::string, int>> defaultInventory;
  };

  // We have a vector of b2Fixtures (declared in DynamicActor abstract class).
  // e.g., to access the weapon fixture: _fixtures[FixtureType::WEAPON]
  enum FixtureType {
    BODY,  // used in combat (with WEAPON fixture)
    FEET,  // used for ground/platform collision detection
    WEAPON,  // used in combat (with BODY fixture)
    FIXTURE_SIZE
  };

  virtual ~Character() override = default;

  virtual bool showOnMap(float x, float y) override = 0;  // DynamicActor
  virtual bool removeFromMap() override;  // DynamicActor
  virtual void update(float delta) override;  // DynamicActor
  virtual void import(const std::string& jsonFileName) override;  // Importable

  virtual void onKilled();
  virtual void onFallToGroundOrPlatform();
  virtual void onMapChanged() {}

  virtual void moveLeft();
  virtual void moveRight();
  virtual void jump();
  virtual void doubleJump();
  virtual void jumpDown();
  virtual void crouch();
  virtual void getUp();

  virtual void sheathWeapon();
  virtual void unsheathWeapon();
  virtual void attack();
  virtual void activateSkill(Skill* skill);
  virtual void knockBack(Character* target, float forceX, float forceY) const;
  virtual void inflictDamage(Character* target, int damage);
  virtual void receiveDamage(Character* source, int damage);
  virtual void lockOn(Character* target);

  virtual void addItem(std::shared_ptr<Item> item, int amount=1);
  virtual void removeItem(Item* item, int amount=1);
  virtual void useItem(Consumable* consumable);
  virtual void equip(Equipment* equipment, bool audio = true);
  virtual void unequip(Equipment::Type equipmentType, bool audio = true);
  virtual void pickupItem(Item* item);
  virtual void discardItem(Item* item, int amount);
  virtual void interact(Interactable* target);
  virtual void addExp(const int exp);

  virtual void addSkill(std::unique_ptr<Skill> skill);
  virtual void removeSkill(Skill* skill);

  int getGoldBalance() const;
  void addGold(const int amount);
  void removeGold(const int amount);

  inline bool isFacingRight() const { return _isFacingRight; }
  inline bool isJumping() const { return _isJumping; }
  inline bool isDoubleJumping() const { return _isDoubleJumping; }
  inline bool isOnPlatform() const { return _isOnPlatform; }
  inline bool isAttacking() const { return _isAttacking; }
  inline bool isUsingSkill() const { return _isUsingSkill; }
  inline bool isCrouching() const { return _isCrouching; }
  inline bool isInvincible() const { return _isInvincible; }
  inline bool isKilled() const { return _isKilled; }
  inline bool isSetToKill() const { return _isSetToKill; }
  inline bool isWeaponSheathed() const { return _isWeaponSheathed; }
  inline bool isSheathingWeapon() const { return _isSheathingWeapon; }
  inline bool isUnsheathingWeapon() const { return _isUnsheathingWeapon; }

  inline void setFacingRight(bool facingRight) { _isFacingRight = facingRight; }
  inline void setJumping(bool jumping) { _isJumping = jumping; }
  inline void setDoubleJumping(bool doubleJumping) { _isDoubleJumping = doubleJumping; }
  inline void setOnPlatform(bool onPlatform) { _isOnPlatform = onPlatform; }
  inline void setAttacking(bool attacking) { _isAttacking = attacking; }
  inline void setUsingSkill(bool usingSkill) { _isUsingSkill = usingSkill; }
  inline void setCrouching(bool crouching) { _isCrouching = crouching; }
  inline void setInvincible(bool invincible) { _isInvincible = invincible; }

  inline Character::Profile& getCharacterProfile() { return _characterProfile; }

  inline std::unordered_set<Character*>& getInRangeTargets() { return _inRangeTargets; }
  inline Character* getLockedOnTarget() const { return _lockedOnTarget; }
  inline void setLockedOnTarget(Character* target) { _lockedOnTarget = target; }
  inline bool isAlerted() const { return _isAlerted; }
  inline void setAlerted(bool alerted) { _isAlerted = alerted; }

  inline std::unordered_set<Item*>& getInRangeItems() { return _inRangeItems; }
  inline std::unordered_set<Interactable*>& getInRangeInteractables() {
    return _inRangeInteractables;
  }

  inline const Inventory& getInventory() const { return _inventory; }
  inline const EquipmentSlots& getEquipmentSlots() const { return _equipmentSlots; }
  int getItemAmount(const std::string& itemJsonFileName) const;

  inline GameMap::Portal* getPortal() const { return _portal; }
  inline void setPortal(GameMap::Portal* portal) { _portal = portal; }

  inline const SkillBook& getSkillBook() const { return _skillBook; }
  std::shared_ptr<Skill> getActiveSkill(Skill* skill) const;
  inline Skill* getCurrentlyUsedSkill() const { return _currentlyUsedSkill; }
  void removeActiveSkill(Skill* skill);

  bool isWaitingForPartyLeader() const;
  std::unordered_set<Character*> getAllies() const;
  inline std::shared_ptr<Party> getParty() const { return _party; }
  inline void setParty(std::shared_ptr<Party> party) { _party = party; }

  int getDamageOutput() const;

 protected:
  explicit Character(const std::string& jsonFileName);

  virtual void regenHealth(int deltaHealth);
  virtual void regenMagicka(int deltaMagicka);
  virtual void regenStamina(int deltaStamina);

  virtual void defineBody(b2BodyType bodyType,
                          float x,
                          float y,
                          short bodyCategoryBits=0,
                          short bodyMaskBits=0,
                          short feetMaskBits=0,
                          short weaponMaskBits=0);

  virtual void defineTexture(const std::string& bodyTextureResDir, float x, float y);

  virtual void loadBodyAnimations(const std::string& bodyTextureResDir);
  virtual void loadEquipmentAnimations(Equipment* equipment);

  void createBodyAnimation(const Character::State state,
                           cocos2d::Animation* fallbackAnimation);
  void createEquipmentAnimation(const Equipment* equipment,
                                const Character::State state,
                                cocos2d::Animation* fallbackAnimation);

  int getExtraAttackAnimationsCount() const;
  cocos2d::Animation* getBodyAttackAnimation() const;
  cocos2d::Animation* getEquipmentAttackAnimation(const Equipment::Type type) const;

  void runAnimation(Character::State state, bool loop=true);
  void runAnimation(Character::State state, const std::function<void ()>& func) const;
  void runAnimation(const std::string& framesName, float interval);

  Character::State getState() const;
  std::optional<std::string> getSfxFileName(const Character::Sfx sfx) const;

  static const std::array<std::string, Character::State::STATE_SIZE> _kCharacterStateStr;
  static const std::array<std::string, Character::Sfx::SFX_SIZE> _kCharacterSfxStr;

  // Characater data.
  Character::Profile _characterProfile;

  // Stats regen timer
  float _statsRegenTimer;
  const int _baseRegenDeltaHealth;
  const int _baseRegenDeltaMagicka;
  const int _baseRegenDeltaStamina;

  // The following variables are used to determine the character's state
  // and run the corresponding animations. Please see Character::update()
  // for the logic.
  Character::State _currentState;
  Character::State _previousState;

  bool _isFacingRight;
  bool _isWeaponSheathed;
  bool _isSheathingWeapon;
  bool _isUnsheathingWeapon;
  bool _isJumpingDisallowed;
  bool _isJumping;
  bool _isDoubleJumping;
  bool _isOnPlatform;
  bool _isAttacking;
  bool _isUsingSkill;
  bool _isCrouching;
  bool _isInvincible;
  bool _isTakingDamage;
  bool _isKilled;
  bool _isSetToKill;

  // The following variables are used to determine combat targets.
  // A character can only inflict damage to another iff the target is
  // within (attack) range.
  std::unordered_set<Character*> _inRangeTargets;
  Character* _lockedOnTarget;
  bool _isAlerted;

  // Nearby items and interactable objects.
  std::unordered_set<Item*> _inRangeItems;
  std::unordered_set<Interactable*> _inRangeInteractables;

  // Character's inventory and equipment slots.
  // These two types are aliased. See the beginning of this class.
  // We use an extra std::map to keep track of each item's count.
  // For each instance of Item, only one copy of Item* is stored.
  Character::Inventory _inventory;
  Character::EquipmentSlots _equipmentSlots;

  // For each item, at most one copy of Item* is kept in memory.
  Item* getExistingItemObj(Item* item) const;
  std::unordered_map<std::string, std::shared_ptr<Item>> _itemMapper;

  // The portal to which this character is near.
  GameMap::Portal* _portal;

  // Currently used skill.
  Character::SkillBook _skillBook;
  std::unordered_map<std::string, std::unique_ptr<Skill>> _skillMapper;
  std::unordered_set<std::shared_ptr<Skill>> _activeSkills;
  Skill* _currentlyUsedSkill;

  // Extra attack animations.
  // The first attack animations is in _bodyAnimations[State::ATTACK],
  // and here's some extra ones.
  const int _kAttackAnimationIdxMax;
  int _attackAnimationIdx;
  std::vector<cocos2d::Animation*> _bodyExtraAttackAnimations;
  std::array<std::vector<cocos2d::Animation*>, Equipment::Type::SIZE> _equipmentExtraAttackAnimations;

  // Besides body sprite and animations (declared in Actor abstract class),
  // there is also a sprite for each equipment slots! Each equipped equipment
  // has their own animation!
  std::array<cocos2d::Sprite*, Equipment::Type::SIZE> _equipmentSprites;
  std::array<cocos2d::SpriteBatchNode*, Equipment::Type::SIZE> _equipmentSpritesheets;
  std::array<std::array<cocos2d::Animation*, Character::State::STATE_SIZE>, Equipment::Type::SIZE>
    _equipmentAnimations;

  // Skill animations
  std::unordered_map<std::string, cocos2d::Animation*> _skillBodyAnimations;

  // Party
  // A character can either:
  // (1) be a leader who has a set of allies/followers, or
  // (2) be a follower of other character
  std::shared_ptr<Party> _party;

  friend class GameState;
};

}  // namespace vigilante

#endif  // VIGILANTE_CHARACTER_H_
