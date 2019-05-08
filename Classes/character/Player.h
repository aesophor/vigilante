#ifndef VIGILANTE_PLAYER_H_
#define VIGILANTE_PLAYER_H_

#include <string>

#include "Box2D/Box2D.h"

#include "map/GameMapManager.h"

namespace vigilante {

class Player {
 public:
  Player(float x, float y);
  virtual ~Player();

  enum State {
    IDLE,
    RUNNING,
    JUMPING,
    FALLING,
    CROUCHING,
    ATTACKING,
    KILLED,
    LAST
  };

  void update(float delta);

  void moveLeft() const;
  void moveRight() const;

  b2Body* getB2Body() const;
  cocos2d::SpriteBatchNode* getSpritesheet() const;

 private:
  void defineBody(float x, float y);
  void defineAnimations(float x, float y);
  cocos2d::Action* defineFrames(const std::string& frameName);

  static const float kBaseMovingSpeed;

  b2Body* _b2body;
  b2Fixture* _bodyFixture;

  cocos2d::SpriteBatchNode* _spritesheet;
  cocos2d::Sprite* _sprite;
  cocos2d::Action* actions[State::LAST];

  Player::State _currentState;
  Player::State _previousState;

  float _stateTimer;
  bool _isFacingRight;
  bool _isJumping;
  bool _isKilled;
  bool _isSetToKill;
};

} // namespace vigilante

#endif // VIGILANTE_PLAYER_H_
