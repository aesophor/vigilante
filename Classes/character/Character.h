#ifndef VIGILANTE_CHARACTER_H_
#define VIGILANTE_CHARACTER_H_

#include <set>
#include <array>
#include <vector>
#include <string>
#include <functional>

#include "cocos2d.h"
#include "Box2D/Box2D.h"

#include "item/Item.h"
#include "item/Equipment.h"

namespace vigilante {

class Character {
 public:
  typedef std::array<std::vector<Item*>, Item::Type::SIZE> Inventory;
  typedef std::array<Equipment*, Equipment::Type::SIZE> EquipmentSlots;

  virtual ~Character();
  virtual void update(float delta);

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

  void setIsJumping(bool isJumping);
  void setIsOnPlatform(bool isOnPlatform);
  void setIsAttacking(bool isAttacking);
  void setIsCrouching(bool isCrouching);
  void setIsInvincible(bool isInvincible);

  std::string getName() const;
  void setName(const std::string& name);
  int getHealth() const;
  int getMagicka() const;
  int getStamina() const;
  int getFullHealth() const;
  int getFullMagicka() const;
  int getFullStamina() const;

  std::set<Character*>& getInRangeTargets();
  Character* getLockedOnTarget() const;
  void setLockedOnTarget(Character* target);
  bool isAlerted() const;
  void setIsAlerted(bool isAlerted);

  std::set<Item*>& getInRangeItems();

  const Inventory& getInventory() const;
  const EquipmentSlots& getEquipmentSlots() const;
  
  b2Body* getB2Body() const;
  cocos2d::Sprite* getSprite() const;
  cocos2d::SpriteBatchNode* getSpritesheet() const;	

  static void setCategoryBits(b2Fixture* fixture, short bits);

 protected:
  Character(const std::string& name, float x, float y);

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

  void defineBody(b2BodyType bodyType,
                  short bodyCategoryBits,
                  short bodyMaskBits,
                  short feetMaskBits,
                  short weaponMaskBits,
                  float x,
                  float y);

  // Derived class should manually implement their own defineTexture method.
  virtual void defineTexture(float x, float y) = 0;
  void loadAnimation(Character::State state, const std::string& frameName, size_t frameCount, float delay=.1f);
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

  // Player stats.
  std::string _name;
  int _str;
  int _dex;
  int _int;
  int _luk;
  int _health;
  int _magicka;
  int _stamina;
  int _fullHealth;
  int _fullMagicka;
  int _fullStamina;

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

  // Character's Box2D body and fixtures.
  // The fixtures are attached to the _b2body, see map/WorldContactListener.cc
  // for the implementation of contact handlers.
  b2Body* _b2body;
  b2Fixture* _bodyFixture; // used in combat (with _weaponFixture)
  b2Fixture* _feetFixture; // used for ground/platform collision detection
  b2Fixture* _weaponFixture; // used in combat (with _bodyFixture)

  // Character's sprites and animations.
  cocos2d::Sprite* _sprite; // child of _spritesheet
  cocos2d::SpriteBatchNode* _spritesheet; // child of MainGameScene
  cocos2d::Animation* _animations[Character::State::SIZE]; // animations cache
};

} // namespace vigilante

#endif // VIGILANTE_CHARACTER_H_
