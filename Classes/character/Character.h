#ifndef VIGILANTE_CHARACTER_H_
#define VIGILANTE_CHARACTER_H_

#include <set>
#include <array>
#include <vector>
#include <string>
#include <functional>

#include "cocos2d.h"
#include "Box2D/Box2D.h"

#include "DynamicActor.h"
#include "character/data/CharacterProfile.h"
#include "item/Item.h"
#include "item/Equipment.h"
#include "map/GameMap.h"

namespace vigilante {

class Character : public DynamicActor {
 public: 
  friend class CharacterProfile;
  virtual ~Character();

  virtual void update(float delta) override;
  virtual void setPosition(float x, float y) override;

  virtual void moveLeft();
  virtual void moveRight();
  virtual void jump();
  virtual void jumpDown();
  virtual void crouch();
  virtual void getUp();

  virtual void sheathWeapon();
  virtual void unsheathWeapon();
  virtual void attack();
  virtual void knockBack(Character* target, float forceX, float forceY) const;
  virtual void inflictDamage(Character* target, int damage);
  virtual void receiveDamage(Character* source, int damage);

  virtual void pickupItem(Item* item);
  virtual void addItem(Item* item);
  virtual void removeItem(Item* item);
  virtual void equip(Equipment* equipment);
  virtual void unequip(Equipment::Type equipmentType);

  bool isFacingRight() const;
  bool isJumping() const;
  bool isOnPlatform() const;
  bool isAttacking() const;
  bool isCrouching() const;
  bool isInvincible() const;
  bool isKilled() const;
  bool isSetToKill() const;
  bool isWeaponSheathed() const;
  bool isSheathingWeapon() const;
  bool isUnsheathingWeapon() const;

  void setJumping(bool jumping);
  void setOnPlatform(bool onPlatform);
  void setAttacking(bool attacking);
  void setCrouching(bool crouching);
  void setInvincible(bool invincible);

  CharacterProfile& getProfile();

  std::set<Character*>& getInRangeTargets();
  Character* getLockedOnTarget() const;
  void setLockedOnTarget(Character* target);
  bool isAlerted() const;
  void setAlerted(bool alerted);

  std::set<Item*>& getInRangeItems();

  GameMap::Portal* getPortal() const;
  void setPortal(GameMap::Portal* portal);

  typedef std::array<std::vector<Item*>, Item::Type::SIZE> Inventory;
  typedef std::array<Equipment*, Equipment::Type::SIZE> EquipmentSlots;
  const Inventory& getInventory() const;
  const EquipmentSlots& getEquipmentSlots() const;
  
  static void setCategoryBits(b2Fixture* fixture, short bits);

 protected:
  Character();

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
    STATE_SIZE
  };

  static const std::array<std::string, Character::State::STATE_SIZE> _kCharacterStateStr;

  virtual void defineBody(b2BodyType bodyType, short bodyCategoryBits, short bodyMaskBits,
                          short feetMaskBits, short weaponMaskBits, float x, float y);
  virtual void defineTexture(const std::string& bodyTextureResPath, float x, float y);

  virtual void loadBodyAnimations(const std::string& bodyTextureResPath);
  virtual void loadEquipmentAnimations(Equipment* equipment);

  void runAnimation(Character::State state, bool loop=true) const;
  void runAnimation(Character::State state, const std::function<void ()>& func) const;

  Character::State getState() const;

  // The following variables are used to determine the character's state
  // and run the corresponding animations. Please see Character::update()
  // for the logic.
  Character::State _currentState;
  Character::State _previousState;
  float _stateTimer;

  bool _isFacingRight;
  bool _isWeaponSheathed;
  bool _isSheathingWeapon;
  bool _isUnsheathingWeapon;
  bool _isJumping;
  bool _isOnPlatform;
  bool _isAttacking;
  bool _isCrouching;
  bool _isInvincible;
  bool _isKilled;
  bool _isSetToKill;

  CharacterProfile _profile;

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
  Character::Inventory _inventory;
  Character::EquipmentSlots _equipmentSlots;

  // The portal to which this character is near.
  GameMap::Portal* _portal;

  // We have a vector of b2Fixtures (declared in Actor abstract class).
  // e.g., to access the weapon fixture: _fixtures[FixtureType::WEAPON]
  enum FixtureType {
    BODY, // used in combat (with WEAPON fixture)
    FEET, // used for ground/platform collision detection
    WEAPON, // used in combat (with BODY fixture)
    FIXTURE_SIZE
  };

  // Extra attack animations.
  // The first attack animations is in _bodyAnimations[State::ATTACK],
  // and here's some extra ones.
  std::array<cocos2d::Animation*, 1> _extraAttackAnimations;

  // Besides body sprite and animations (declared in Actor abstract class),
  // there is also a sprite for each equipment slots! Each equipped equipment
  // has their own animation!
  std::array<cocos2d::Sprite*, Equipment::Type::SIZE> _equipmentSprites;
  std::array<cocos2d::SpriteBatchNode*, Equipment::Type::SIZE> _equipmentSpritesheets;
  std::array<std::array<cocos2d::Animation*, Character::State::STATE_SIZE>, Equipment::Type::SIZE> _equipmentAnimations;
};

} // namespace vigilante

#endif // VIGILANTE_CHARACTER_H_
