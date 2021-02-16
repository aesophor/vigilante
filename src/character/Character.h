// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_CHARACTER_H_
#define VIGILANTE_CHARACTER_H_

#include <set>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include <functional>

#include <cocos2d.h>
#include <Box2D/Box2D.h>
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
 public: 
  using Inventory = std::array<SetVector<Item*>, Item::Type::SIZE>;
  using EquipmentSlots = std::array<Equipment*, Equipment::Type::SIZE>;
  using SkillBook = std::array<SetVector<Skill*>, Skill::Type::SIZE>;

  struct Profile final {
    explicit Profile(const std::string& jsonFileName);
    ~Profile() = default;

    std::string jsonFileName;
    std::string textureResDir;
    float spriteOffsetX;
    float spriteOffsetY;
    float spriteScaleX;
    float spriteScaleY;
    std::vector<float> frameInterval;

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

  virtual ~Character() = default;

  virtual bool showOnMap(float x, float y) override = 0;  // DynamicActor
  virtual bool removeFromMap() override;  // DynamicActor
  virtual void update(float delta) override;  // DynamicActor
  virtual void import(const std::string& jsonFileName) override;  // Importable

  virtual void onKilled();

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
  virtual void equip(Equipment* equipment);
  virtual void unequip(Equipment::Type equipmentType);
  virtual void pickupItem(Item* item);
  virtual void discardItem(Item* item, int amount);
  virtual void interact(Interactable* target);
  virtual void addExp(const int exp);
  
  virtual void addSkill(std::unique_ptr<Skill> skill);
  virtual void removeSkill(Skill* skill);

  int getGoldBalance() const;
  void addGold(const int amount);
  void removeGold(const int amount);

  bool isFacingRight() const;
  bool isJumping() const;
  bool isDoubleJumping() const;
  bool isOnPlatform() const;
  bool isAttacking() const;
  bool isUsingSkill() const;
  bool isCrouching() const;
  bool isInvincible() const;
  bool isKilled() const;
  bool isSetToKill() const;
  bool isWeaponSheathed() const;
  bool isSheathingWeapon() const;
  bool isUnsheathingWeapon() const;

  void setJumping(bool jumping);
  void setDoubleJumping(bool doubleJumping);
  void setOnPlatform(bool onPlatform);
  void setAttacking(bool attacking);
  void setUsingSkill(bool usingSkill);
  void setCrouching(bool crouching);
  void setInvincible(bool invincible);

  Character::Profile& getCharacterProfile();

  std::set<Character*>& getInRangeTargets();
  Character* getLockedOnTarget() const;
  void setLockedOnTarget(Character* target);
  bool isAlerted() const;
  void setAlerted(bool alerted);

  std::set<Item*>& getInRangeItems();

  const Inventory& getInventory() const;
  const EquipmentSlots& getEquipmentSlots() const;
  int getItemAmount(const std::string& itemName) const;

  Interactable* getInteractableObject() const;
  void setInteractableObject(Interactable* interactableObject);

  GameMap::Portal* getPortal() const;
  void setPortal(GameMap::Portal* portal);

  const SkillBook& getSkillBook() const;
  std::unordered_set<std::shared_ptr<Skill>>& getActiveSkills();
  Skill* getCurrentlyUsedSkill() const;

  bool isWaitingForPartyLeader() const;
  std::unordered_set<Character*> getAllies() const;
  std::shared_ptr<Party> getParty() const;
  void setParty(std::shared_ptr<Party> party);

  int getDamageOutput() const;
  

 protected:
  explicit Character(const std::string& jsonFileName);

  virtual void regenHealth(int deltaHealth);
  virtual void regenMagicka(int deltaMagicka);
  virtual void regenStamina(int deltaStamina);

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

  static const std::array<std::string, Character::State::STATE_SIZE> _kCharacterStateStr;

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

  int getExtraAttackAnimationsCount() const;
  cocos2d::Animation* getBodyAttackAnimation() const;
  cocos2d::Animation* getEquipmentAttackAnimation(const Equipment::Type type) const;

  void runAnimation(Character::State state, bool loop=true);
  void runAnimation(Character::State state, const std::function<void ()>& func) const;
  void runAnimation(const std::string& framesName, float interval);

  Character::State getState() const;


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
  std::set<Character*> _inRangeTargets;
  Character* _lockedOnTarget;
  bool _isAlerted;

  // When player are close enough to the items dropped in the world,
  // the pointers to the items are stored here.
  std::set<Item*> _inRangeItems;

  // Character's inventory and equipment slots.
  // These two types are aliased. See the beginning of this class.
  // We use an extra std::map to keep track of each item's count.
  // For each instance of Item, only one copy of Item* is stored.
  Character::Inventory _inventory;
  Character::EquipmentSlots _equipmentSlots;

  // For each item, at most one copy of Item* is kept in memory.
  Item* getExistingItemObj(Item* item) const;
  std::unordered_map<std::string, std::shared_ptr<Item>> _itemMapper;   


  // The interactable object / portal to which this character is near.
  Interactable* _interactableObject;
  GameMap::Portal* _portal;


  // Currently used skill.
  Character::SkillBook _skillBook;
  std::unordered_map<std::string, std::unique_ptr<Skill>> _skillMapper;

  //std::vector<std::unique_ptr<Skill>> _skills;
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
};

}  // namespace vigilante

#endif  // VIGILANTE_CHARACTER_H_
