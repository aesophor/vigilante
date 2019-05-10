#ifndef VIGILANTE_PLAYER_H_
#define VIGILANTE_PLAYER_H_

#include <string>
#include <vector>

#include "Box2D/Box2D.h"

#include "map/GameMapManager.h"

namespace vigilante {

class Player {
 public:
  Player(float x, float y);
  virtual ~Player();

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

  void update(float delta);

  void moveLeft();
  void moveRight();
  void jump();
  void jumpDown();
  void crouch();
  void getUp();
  void sheathWeapon();
  void unsheathWeapon();
  void attack();

  b2Body* getB2Body() const;
  cocos2d::Sprite* getSprite() const;
  cocos2d::SpriteBatchNode* getSpritesheet() const;

  bool isJumping() const;
  bool isCrouching() const;
  bool isWeaponSheathed() const;
  bool isSheathingWeapon() const;
  bool isUnsheathingWeapon() const;
  bool isOnPlatform() const;

  void setIsJumping(bool isJumping);
  void setIsCrouching(bool isCrouching);
  void setIsOnPlatform(bool isOnPlatform);

 private:
  void defineBody(float x, float y);
  void defineTexture(float x, float y);

  void loadAnimation(State state, const std::string& frameName, size_t frameCount, float delay=0.1f);
  void runAnimation(State state, bool loop=true) const;

  State getState() const;

  static const float kBaseMovingSpeed;

  b2Body* _b2body;
  b2Fixture* _bodyFixture;
  b2Fixture* _feetFixture;
  b2Fixture* _weaponFixture;

  cocos2d::Sprite* _sprite;
  cocos2d::SpriteBatchNode* _spritesheet;
  cocos2d::Animation* _animations[State::LAST];

  Player::State _currentState;
  Player::State _previousState;

  float _stateTimer;
  bool _isFacingRight;
  bool _isWeaponSheathed;
  bool _isSheathingWeapon;
  bool _isUnsheathingWeapon;
  bool _isJumping;
  bool _isOnPlatform;
  bool _isAttacking;
  bool _isCrouching;
  bool _isKilled;
  bool _isSetToKill;

  // Combat
};

} // namespace vigilante

#endif // VIGILANTE_PLAYER_H_
