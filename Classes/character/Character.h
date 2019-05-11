#ifndef VIGILANTE_CHARACTER_H_
#define VIGILANTE_CHARACTER_H_

#include <string>
#include <vector>
#include <functional>

#include "cocos2d.h"
#include "Box2D/Box2D.h"


namespace vigilante {

class Character {
 public:
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

  bool isFacingRight() const;
	bool isJumping() const;
	bool isOnPlatform() const;
	bool isAttacking() const;
	bool isCrouching() const;
  bool isInvincible() const;
  bool isKilled() const;
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
    LAST
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
	void loadAnimation(State state, const std::string& frameName, size_t frameCount, float delay=.1f);
	void runAnimation(State state, bool loop=true) const;
  void runAnimation(State state, const std::function<void ()>& func) const;

	State getState() const;
	static const float _kBaseMovingSpeed;

  State _currentState;
	State _previousState;
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

  std::string _name;
  int _str;
  int _dex;
  int _int;
  int _luk;
  int _health;
  int _magicka;
  int _stamina;
  
	b2Body* _b2body;
	b2Fixture* _bodyFixture;
	b2Fixture* _feetFixture;
	b2Fixture* _weaponFixture;

	cocos2d::Sprite* _sprite;
	cocos2d::SpriteBatchNode* _spritesheet;
	cocos2d::Animation* _animations[State::LAST];
};

} // namespace vigilante

#endif // VIGILANTE_CHARACTER_H_
