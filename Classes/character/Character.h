#ifndef VIGILANTE_CHARACTER_H_
#define VIGILANTE_CHARACTER_H_

#include <set>
#include <array>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <functional>

#include "cocos2d.h"
#include "Box2D/Box2D.h"

#include "DynamicActor.h"
#include "Importable.h"
#include "Regeneratable.h"
#include "Interactable.h"
#include "item/Item.h"
#include "item/Equipment.h"
#include "item/Consumable.h"
#include "map/GameMap.h"
#include "skill/Skill.h"

namespace vigilante {

class Character : public DynamicActor, public Importable, public Regeneratable {
 public: 
  struct Profile {
    Profile(const std::string& jsonFileName);
    virtual ~Profile() = default;

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
  };

  // We have a vector of b2Fixtures (declared in DynamicActor abstract class).
  // e.g., to access the weapon fixture: _fixtures[FixtureType::WEAPON]
  enum FixtureType {
    BODY, // used in combat (with WEAPON fixture)
    FEET, // used for ground/platform collision detection
    WEAPON, // used in combat (with BODY fixture)
    FIXTURE_SIZE
  };

  virtual ~Character();

  virtual void update(float delta) override; // DynamicActor
  virtual void setPosition(float x, float y) override; // StaticActor
  virtual void removeFromMap() override; // DynamicActor
  virtual void import(const std::string& jsonFileName) override; // Importable

  virtual void moveLeft();
  virtual void moveRight();
  virtual void jump();
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

  virtual void addItem(Item* item);
  virtual void removeItem(Item* item);
  virtual void useItem(Consumable* consumable);
  virtual void equip(Equipment* equipment);
  virtual void unequip(Equipment::Type equipmentType);
  virtual void pickupItem(Item* item);
  virtual void discardItem(Item* item);
  virtual void interact(Interactable* target);

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

  Interactable* getInteractableObject() const;
  void setInteractableObject(Interactable* interactableObject);

  GameMap::Portal* getPortal() const;
  void setPortal(GameMap::Portal* portal);

  Skill* getCurrentlyUsedSkill() const;

  typedef std::array<std::map<Item*, int>, Item::Type::SIZE> Inventory;
  typedef std::array<Equipment*, Equipment::Type::SIZE> EquipmentSlots;
  const Inventory& getInventory() const;
  const EquipmentSlots& getEquipmentSlots() const;

  int getDamageOutput() const;
  
  static void setCategoryBits(b2Fixture* fixture, short bits);

 protected:
  Character(const std::string& jsonFileName);

  virtual void regenHealth(int deltaHealth) override; // Regeneratable
  virtual void regenMagicka(int deltaMagicka) override; // Regeneratable
  virtual void regenStamina(int deltaStamina) override; // Regeneratable

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

  virtual void defineBody(b2BodyType bodyType, short bodyCategoryBits, short bodyMaskBits,
                          short feetMaskBits, short weaponMaskBits, float x, float y);
  virtual void defineTexture(const std::string& bodyTextureResDir, float x, float y);

  virtual void loadBodyAnimations(const std::string& bodyTextureResDir);
  virtual void loadEquipmentAnimations(Equipment* equipment);

  void runAnimation(Character::State state, bool loop=true) const;
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
  bool _isJumping;
  bool _isDoubleJumping;
  bool _isOnPlatform;
  bool _isAttacking;
  bool _isUsingSkill;
  bool _isCrouching;
  bool _isInvincible;
  bool _isKilled;
  bool _isSetToKill;

  // The following variables are used to determine combat targets.
  // A character can only inflict damage to another iff the target is
  // within (attack) range.
  std::set<Character*> _inRangeTargets;
  Character* _lockedOnTarget;
  bool _isAlerted;

  // When player are close enough to an items dropped in the world,
  // their pointers are stored here.
  std::set<Item*> _inRangeItems;

  // Character's inventory and equipment slots.
  // These two types are `typedef`ed. See the beginning of this class.
  // We use an extra std::map to keep track of each item's count.
  // For each instance of Item, only one copy of Item* is stored.
  Character::Inventory _inventory;
  Character::EquipmentSlots _equipmentSlots;

  // For each item, at most one copy of Item* is kept in memory.
  Item* getExistingItemObj(Item* item) const;
  std::unordered_map<std::string, Item*> _itemMapper;   


  // The interactable object / portal to which this character is near.
  Interactable* _interactableObject;
  GameMap::Portal* _portal;

  // Currently used skill.
  Skill* _currentlyUsedSkill;

  // Extra attack animations.
  // The first attack animations is in _bodyAnimations[State::ATTACK],
  // and here's some extra ones.
  std::array<cocos2d::Animation*, 1> _bodyExtraAttackAnimations;
  std::array<std::array<cocos2d::Animation*, Character::State::STATE_SIZE>, Equipment::Type::SIZE> _equipmentExtraAttackAnimations;

  // Besides body sprite and animations (declared in Actor abstract class),
  // there is also a sprite for each equipment slots! Each equipped equipment
  // has their own animation!
  std::array<cocos2d::Sprite*, Equipment::Type::SIZE> _equipmentSprites;
  std::array<cocos2d::SpriteBatchNode*, Equipment::Type::SIZE> _equipmentSpritesheets;
  std::array<std::array<cocos2d::Animation*, Character::State::STATE_SIZE>, Equipment::Type::SIZE> _equipmentAnimations;

  // Skill animations
  std::unordered_map<std::string, cocos2d::Animation*> _skillBodyAnimations;
};

} // namespace vigilante

#endif // VIGILANTE_CHARACTER_H_
