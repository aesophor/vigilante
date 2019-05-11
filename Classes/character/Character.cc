#include "Character.h"

#include "map/GameMapManager.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/CallbackUtil.h"
#include "util/CategoryBits.h"
#include "util/Constants.h"

using std::string;
using std::vector;
using cocos2d::Vector;
using cocos2d::Director;
using cocos2d::Repeat;
using cocos2d::RepeatForever;
using cocos2d::Animation;
using cocos2d::Animate;
using cocos2d::Action;
using cocos2d::Sprite;
using cocos2d::SpriteFrame;
using cocos2d::SpriteFrameCache;
using cocos2d::SpriteBatchNode;


namespace vigilante {

const float Character::_kBaseMovingSpeed = .25f;

Character::Character(const string& name, float x, float y)
    : _currentState(State::IDLE_SHEATHED),
      _previousState(State::IDLE_SHEATHED),
      _stateTimer(),
      _isFacingRight(true),
      _isWeaponSheathed(true),
      _isSheathingWeapon(),
      _isUnsheathingWeapon(),
      _isJumping(),
      _isOnPlatform(),
      _isAttacking(),
      _isCrouching(),
      _isKilled(),
      _isSetToKill(),
      _name(name),
      _b2body(),
      _bodyFixture(),
      _feetFixture(),
      _weaponFixture(),
      _sprite(),
      _spritesheet() {}

Character::~Character() {
  if (_spritesheet) {
    _spritesheet->release();
  }
}


void Character::update(float delta) {
  if (_isKilled) return;

  _previousState = _currentState;
  _currentState = getState();

  // If there's a change in character's state, update its animation accordingly.
  if (_previousState != _currentState) {
    switch(_currentState) {
      case State::RUNNING_SHEATHED:
        runAnimation(State::RUNNING_SHEATHED, true);
        break;
      case State::RUNNING_UNSHEATHED:
        runAnimation(State::RUNNING_UNSHEATHED, true);
        break;
      case State::JUMPING_SHEATHED:
        runAnimation(State::JUMPING_SHEATHED, false);
        break;
      case State::JUMPING_UNSHEATHED:
        runAnimation(State::JUMPING_UNSHEATHED, false);
        break;
      case State::FALLING_SHEATHED:
        runAnimation(State::FALLING_SHEATHED, false);
        break;
      case State::FALLING_UNSHEATHED:
        runAnimation(State::FALLING_UNSHEATHED, false);
        break;
      case State::CROUCHING_SHEATHED:
        runAnimation(State::CROUCHING_SHEATHED, false);
        break;
      case State::CROUCHING_UNSHEATHED:
        runAnimation(State::CROUCHING_UNSHEATHED, false);
        break;
      case State::SHEATHING_WEAPON:
        runAnimation(State::SHEATHING_WEAPON, false);
        break;
      case State::UNSHEATHING_WEAPON:
        runAnimation(State::UNSHEATHING_WEAPON, false);
        break;
      case State::ATTACKING:
        runAnimation(State::ATTACKING, false);
        break;
      case State::KILLED:
        runAnimation(State::KILLED, false);
        break;
      case State::IDLE_SHEATHED:
        runAnimation(State::IDLE_SHEATHED, true);
        break;
      case State::IDLE_UNSHEATHED: // fall through
      default:
        runAnimation(State::IDLE_UNSHEATHED, true);
        break;
		}
	}

  if (!_isFacingRight && !_sprite->isFlippedX()) {
    _sprite->setFlippedX(true);
    b2CircleShape* shape = static_cast<b2CircleShape*>(_weaponFixture->GetShape());
    shape->m_p = {-15.0f / kPpm, 0};
  } else if (_isFacingRight && _sprite->isFlippedX()) {
    _sprite->setFlippedX(false);
    b2CircleShape* shape = static_cast<b2CircleShape*>(_weaponFixture->GetShape());
    shape->m_p = {15.0f / kPpm, 0};
  }

  _stateTimer = (_currentState != _previousState) ? 0 : _stateTimer + delta;

  //cocos2d::log("[Character::update] x=%f y=%f", _b2body->GetPosition().x, _b2body->GetPosition().y);
  _sprite->setPosition(_b2body->GetPosition().x * kPpm, _b2body->GetPosition().y * kPpm + 5);
}

void Character::defineBody(b2BodyType bodyType,
                           short bodyCategoryBits,
                           short bodyMaskBits,
                           short feetMaskBits,
                           short weaponMaskBits,
                           float x,
                           float y) {
  b2World* world = GameMapManager::getInstance()->getWorld();
  b2BodyBuilder bodyBuilder(world);

  _b2body = bodyBuilder.type(bodyType)
    .position(x, y, 1)
    .buildBody();

  // Create body fixture.
  // Fixture position in box2d is relative to b2body's position.
  float scaleFactor = Director::getInstance()->getContentScaleFactor();
  b2Vec2 vertices[4];
  vertices[0] = {-5 / scaleFactor,  20 / scaleFactor};
  vertices[1] = { 5 / scaleFactor,  20 / scaleFactor};
  vertices[2] = {-5 / scaleFactor, -14 / scaleFactor};
  vertices[3] = { 5 / scaleFactor, -14 / scaleFactor};

  bodyBuilder.newPolygonFixture(vertices, 4, kPpm)
    .categoryBits(bodyCategoryBits)
    .maskBits(bodyMaskBits)
    .setUserData(this)
    .buildFixture();


  // Create feet fixture.
  b2Vec2 feetVertices[4];
  feetVertices[0] = {(-5 + 1) / scaleFactor, 0};
  feetVertices[1] = {( 5 - 1) / scaleFactor, 0};
  feetVertices[2] = {(-5 + 1) / scaleFactor, (-14 - 1) / scaleFactor};
  feetVertices[3] = {( 5 - 1) / scaleFactor, (-14 - 1) / scaleFactor};

  _feetFixture = bodyBuilder.newPolygonFixture(feetVertices, 4, kPpm)
    .categoryBits(category_bits::kFeet)
    .maskBits(feetMaskBits)
    .setUserData(this)
    .buildFixture();


  // Create weapon fixture.
  _weaponFixture = bodyBuilder.newCircleFixture({15.0f, 0}, 15.0f, kPpm)
    .categoryBits(category_bits::kMeleeWeapon)
    .maskBits(weaponMaskBits)
    .setSensor(true)
    .setUserData(this)
    .buildFixture();
}

void Character::loadAnimation(State state, const string& frameName, size_t frameCount, float delay) {
  SpriteFrameCache* frameCache = SpriteFrameCache::getInstance();

  Vector<SpriteFrame*> frames;
  for (size_t i = 0; i < frameCount; i++) {
    string name = frameName + "/" + frameName + std::to_string(i) + ".png";
    frames.pushBack(frameCache->getSpriteFrameByName(name));
  }
  _animations[state] = Animation::createWithSpriteFrames(frames, delay);
  _animations[state]->retain();
}

void Character::runAnimation(State state, bool loop) const {
  _sprite->stopAllActions();

  auto animation = Animate::create(_animations[state]);
  if (loop) {
    _sprite->runAction(RepeatForever::create(animation));
  } else {
    _sprite->runAction(Repeat::create(animation, 1));
  }
}


Character::State Character::getState() const {
  if (_isSetToKill) {
    return State::KILLED;
  } else if (_isAttacking) {
    return State::ATTACKING;
  } else if (_isSheathingWeapon) {
    return State::SHEATHING_WEAPON;
  } else if (_isUnsheathingWeapon) {
    return State::UNSHEATHING_WEAPON;
  } else if (_isJumping) {
    return (_isWeaponSheathed) ? State::JUMPING_SHEATHED : State::JUMPING_UNSHEATHED;
  } else if (_b2body->GetLinearVelocity().y < -.5f) {
    return (_isWeaponSheathed) ? State::FALLING_SHEATHED : State::FALLING_UNSHEATHED;
  } else if (_isCrouching) {
    return (_isWeaponSheathed) ? State::CROUCHING_SHEATHED : State::CROUCHING_UNSHEATHED;
  } else if (std::abs(_b2body->GetLinearVelocity().x) > .01f) {
    return (_isWeaponSheathed) ? State::RUNNING_SHEATHED : State::RUNNING_UNSHEATHED;
  } else {
    return (_isWeaponSheathed) ? State::IDLE_SHEATHED : State::IDLE_UNSHEATHED;
  }
}


void Character::moveLeft() {
  _isFacingRight = false;
  if (_b2body->GetLinearVelocity().x >= -_kBaseMovingSpeed * 2) {
    _b2body->ApplyLinearImpulse({-_kBaseMovingSpeed, 0}, _b2body->GetWorldCenter(), true);
  }
}

void Character::moveRight() {
  _isFacingRight = true;
  if (_b2body->GetLinearVelocity().x <= _kBaseMovingSpeed * 2) {
    _b2body->ApplyLinearImpulse({_kBaseMovingSpeed, 0}, _b2body->GetWorldCenter(), true);
  }
}

void Character::jump() {
  if (!_isJumping) {
    _isJumping = true;
    _b2body->ApplyLinearImpulse({0, 3.0f}, _b2body->GetWorldCenter(), true);
  }
}

void Character::jumpDown() {
  if (_isOnPlatform) {
    _feetFixture->SetSensor(true);

    callback_util::runAfter([&](){
      _feetFixture->SetSensor(false);
    }, .25f);
  }
}

void Character::crouch() {
  _isCrouching = true;
}

void Character::getUp() {
  _isCrouching = false;
}

void Character::sheathWeapon() {
  _isSheathingWeapon = true;

  callback_util::runAfter([&](){
    _isSheathingWeapon = false;
    _isWeaponSheathed = true;
  }, .8f);
}

void Character::unsheathWeapon() {
  _isUnsheathingWeapon = true;

  callback_util::runAfter([&](){
    _isUnsheathingWeapon = false;
    _isWeaponSheathed = false;
  }, .8f);
}

void Character::attack() {
  if (_isWeaponSheathed) {
    unsheathWeapon();
    return;
  }

  _isAttacking = true;

  callback_util::runAfter([&](){
    _isAttacking = false;
  }, .5f);
}


b2Body* Character::getB2Body() const {
  return _b2body;
}

Sprite* Character::getSprite() const {
  return _sprite;
}

SpriteBatchNode* Character::getSpritesheet() const {
  return _spritesheet;
}

bool Character::isJumping() const {
  return _isJumping;
}

bool Character::isCrouching() const {
  return _isCrouching;
}

bool Character::isAttacking() const {
  return _isAttacking;
}

bool Character::isWeaponSheathed() const {
  return _isWeaponSheathed;
}

bool Character::isSheathingWeapon() const {
  return _isSheathingWeapon;
}

bool Character::isUnsheathingWeapon() const {
  return _isUnsheathingWeapon;
}

bool Character::isOnPlatform() const {
  return _isOnPlatform;
}


void Character::setIsJumping(bool isJumping) {
  _isJumping = isJumping;
}

void Character::setIsCrouching(bool isCrouching) {
  _isCrouching = isCrouching;
}

void Character::setIsAttacking(bool isAttacking) {
  _isAttacking = isAttacking;
}

void Character::setIsOnPlatform(bool isOnPlatform) {
  _isOnPlatform = isOnPlatform;
}


string Character::getName() const {
  return _name;
}

void Character::setName(const string& name) {
  _name = name;
}

} // namespace vigilante
