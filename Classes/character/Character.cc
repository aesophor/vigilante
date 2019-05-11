#include "Character.h"

#include "map/GameMapManager.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/CallbackUtil.h"
#include "util/CategoryBits.h"
#include "util/Constants.h"

using std::set;
using std::string;
using std::function;
using cocos2d::Vector;
using cocos2d::Director;
using cocos2d::Sequence;
using cocos2d::CallFunc;
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
      _isInvincible(),
      _isKilled(),
      _isSetToKill(),
      _lockedOnTarget(),
      _isAlerted(),
      _name(name),
      _str(5),
      _dex(5),
      _int(5),
      _luk(5),
      _health(100),
      _magicka(100),
      _stamina(100),
      _fullHealth(100),
      _fullMagicka(100),
      _fullStamina(100),
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

  _b2body->GetWorld()->DestroyBody(_b2body);
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
        runAnimation(State::KILLED, [&](){
          // Execute after the KILLED animation is finished.
          GameMapManager::getInstance()->getWorld()->DestroyBody(_b2body);
          _isKilled = true;
        });
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
    .position(x, y, kPpm)
    .buildBody();

  // Create body fixture.
  // Fixture position in box2d is relative to b2body's position.
  float scaleFactor = Director::getInstance()->getContentScaleFactor();
  b2Vec2 vertices[4];
  vertices[0] = {-5 / scaleFactor,  20 / scaleFactor};
  vertices[1] = { 5 / scaleFactor,  20 / scaleFactor};
  vertices[2] = {-5 / scaleFactor, -14 / scaleFactor};
  vertices[3] = { 5 / scaleFactor, -14 / scaleFactor};

  _bodyFixture = bodyBuilder.newPolygonFixture(vertices, 4, kPpm)
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
  Action* action = nullptr;
  if (loop) {
    action = RepeatForever::create(animation);
  } else {
    action = Repeat::create(animation, 1);
  }
  action->setTag(state);
  _sprite->runAction(action);
}

void Character::runAnimation(State state, const function<void ()>& func) const {
  _sprite->stopAllActions();

  auto animation = Animate::create(_animations[state]);
  auto callback = CallFunc::create(func);
  _sprite->runAction(Sequence::createWithTwoActions(animation, callback));
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

  if (!_inRangeTargets.empty()) {
    _lockedOnTarget = *_inRangeTargets.begin();

    if (!_lockedOnTarget->isInvincible()) {
      inflictDamage(_lockedOnTarget, 25);
      float knockBackForceX = (isFacingRight()) ? .5f : -.5f; // temporary
      float knockBackForceY = 1.0f; // temporary
      knockBack(_lockedOnTarget, knockBackForceX, knockBackForceY);
    }
  }
}

void Character::knockBack(Character* target, float forceX, float forceY) const {
  b2Body* b2body = target->getB2Body();
  b2body->ApplyLinearImpulse({forceX, forceY}, b2body->GetWorldCenter(), true);
}

void Character::inflictDamage(Character* target, int damage) {
  target->receiveDamage(this, damage);
}

void Character::receiveDamage(Character* source, int damage) {
  if (_isInvincible) {
    return;
  }

  _health -= damage;
  if (_health <= 0) {
    source->getInRangeTargets().erase(this);
    Character::setCategoryBits(_bodyFixture, category_bits::kDestroyed);
    _isSetToKill = true;
    // TODO: play killed sound.
  } else {
    // TODO: play hurt sound.
  }
}


bool Character::isFacingRight() const {
  return _isFacingRight;
}

bool Character::isJumping() const {
  return _isJumping;
}

bool Character::isOnPlatform() const {
  return _isOnPlatform;
}

bool Character::isAttacking() const {
  return _isAttacking;
}

bool Character::isCrouching() const {
  return _isCrouching;
}

bool Character::isInvincible() const {
  return _isInvincible;
}

bool Character::isKilled() const {
  return _isKilled;
}

bool Character::isSetToKill() const {
  return _isSetToKill;
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


void Character::setIsJumping(bool isJumping) {
  _isJumping = isJumping;
}

void Character::setIsOnPlatform(bool isOnPlatform) {
  _isOnPlatform = isOnPlatform;
}

void Character::setIsAttacking(bool isAttacking) {
  _isAttacking = isAttacking;
}

void Character::setIsCrouching(bool isCrouching) {
  _isCrouching = isCrouching;
}

void Character::setIsInvincible(bool isInvincible) {
  _isInvincible = isInvincible;
}


set<Character*>& Character::getInRangeTargets() {
  return _inRangeTargets;
}

Character* Character::getLockedOnTarget() const {
  return _lockedOnTarget;
}

void Character::setLockedOnTarget(Character* target) {
  _lockedOnTarget = target;
}

bool Character::isAlerted() const {
  return _isAlerted;
}

void Character::setIsAlerted(bool isAlerted) {
  _isAlerted = isAlerted;
}


string Character::getName() const {
  return _name;
}

void Character::setName(const string& name) {
  _name = name;
}

int Character::getHealth() const {
  return _health;
}

int Character::getMagicka() const {
  return _magicka;
}

int Character::getStamina() const {
  return _stamina;
}

int Character::getFullHealth() const {
  return _fullHealth;
}

int Character::getFullMagicka() const {
  return _fullMagicka;
}

int Character::getFullStamina() const {
  return _fullStamina;
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


void Character::setCategoryBits(b2Fixture* fixture, short bits) {
  b2Filter filter;
  filter.categoryBits = bits;
  fixture->SetFilterData(filter);
}

} // namespace vigilante
