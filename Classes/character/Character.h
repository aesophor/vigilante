#ifndef VIGILANTE_CHARACTER_H_
#define VIGILANTE_CHARACTER_H_

#include <set>
#include <array>
#include <vector>
#include <string>
#include <functional>

#include "cocos2d.h"
#include "Box2D/Box2D.h"

#include "character/data/CharacterProfile.h"
#include "item/Item.h"
#include "item/Equipment.h"
#include "map/GameMap.h"

namespace vigilante {

class Character {
 public: 
  friend class CharacterProfile;
  virtual ~Character();
  virtual void update(float delta);

  virtual void reposition(float x, float y);
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
  
  b2Body* getBody() const;
  cocos2d::Sprite* getBodySprite() const;
  cocos2d::SpriteBatchNode* getBodySpritesheet() const;	

  static void setCategoryBits(b2Fixture* fixture, short bits);

 protected:
  Character(float x, float y);

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
    SIZE
  };

  static const std::array<std::string, Character::State::SIZE> _kCharacterStateStr;

  void defineBody(b2BodyType bodyType,
                  short bodyCategoryBits,
                  short bodyMaskBits,
                  short feetMaskBits,
                  short weaponMaskBits,
                  float x,
                  float y);

  virtual void defineTexture(const std::string& bodyTextureResPath, float x, float y);
  virtual void loadBodyAnimations(const std::string& bodyTextureResPath);
  virtual void loadEquipmentAnimations(Equipment* equipment);

  // Load the animation of the specified state into the given animation array.
  cocos2d::Animation* createAnimation(const std::string& textureResPath, std::string frameName, float delay=.1f) const;

  void runAnimation(Character::State state, bool loop=true) const;
  void runAnimation(Character::State state, const std::function<void ()>& func) const;

  Character::State getState() const;
  static const float _kBaseMovingSpeed;

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

  // Character's Box2D body and fixtures.
  // The fixtures are attached to the _b2body, see map/WorldContactListener.cc
  // for the implementation of contact handlers.
  b2Body* _body;
  b2Fixture* _bodyFixture; // used in combat (with _weaponFixture)
  b2Fixture* _feetFixture; // used for ground/platform collision detection
  b2Fixture* _weaponFixture; // used in combat (with _bodyFixture)

  // Character's body and equipment sprites.
  cocos2d::Sprite* _bodySprite; // child of _spritesheet
  std::array<cocos2d::Sprite*, Equipment::Type::SIZE> _equipmentSprites;

  // Character's body and equipment spritesheets.
  cocos2d::SpriteBatchNode* _bodySpritesheet; // child of MainGameScene
  std::array<cocos2d::SpriteBatchNode*, Equipment::Type::SIZE> _equipmentSpritesheets;

  // Character's body and equipment animations.
  // AnimationArray is `typedef`ed. See the beginning of this class.
  typedef std::array<cocos2d::Animation*, Character::State::SIZE> AnimationArray;
  AnimationArray _bodyAnimations;
  std::array<AnimationArray, Equipment::Type::SIZE> _equipmentAnimations;
};

} // namespace vigilante

#endif // VIGILANTE_CHARACTER_H_
