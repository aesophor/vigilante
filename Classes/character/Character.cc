#include "Character.h"

#include "GameAssetManager.h"
#include "map/GameMapManager.h"
#include "ui/damage/FloatingDamageManager.h"
#include "ui/notification/NotificationManager.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/CallbackUtil.h"
#include "util/CategoryBits.h"
#include "util/Constants.h"

using std::set;
using std::vector;
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
      _lockedOnTarget(),
      _isAlerted(),
      _inventory(),
      _equipmentSlots(),
      _portal(),
      _b2body(),
      _bodyFixture(),
      _feetFixture(),
      _weaponFixture(),
      _bodySprite(),
      _bodySpritesheet() {}

Character::~Character() {
  _b2body->GetWorld()->DestroyBody(_b2body);

  // Delete all items from inventory and equipment slots.
  for (const auto& items : _inventory) { // vector<Item*>
    for (const auto item : items) { // Item*
      delete item;
    }
  }
  for (const auto equipment : _equipmentSlots) {
    delete equipment;
  }
}


void Character::update(float delta) {
  if (_isKilled) return;

  _previousState = _currentState;
  _currentState = getState();

  // If there's a change in character's state, run the corresponding animation.
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
        runAnimation(State::KILLED, [=]() {
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
  _stateTimer = (_currentState != _previousState) ? 0 : _stateTimer + delta;

  // Flip the sprite if needed.
  if (!_isFacingRight && !_bodySprite->isFlippedX()) {
    _bodySprite->setFlippedX(true);
    b2CircleShape* shape = static_cast<b2CircleShape*>(_weaponFixture->GetShape());
    shape->m_p = {-15.0f / kPpm, 0};
  } else if (_isFacingRight && _bodySprite->isFlippedX()) {
    _bodySprite->setFlippedX(false);
    b2CircleShape* shape = static_cast<b2CircleShape*>(_weaponFixture->GetShape());
    shape->m_p = {15.0f / kPpm, 0};
  }

  // Sync the body sprite with its b2body.
  b2Vec2 b2bodyPos = _b2body->GetPosition();
  _bodySprite->setPosition(b2bodyPos.x * kPpm, b2bodyPos.y * kPpm + 5);

  // Sync the equipment sprites with its b2body.
  for (int i = 0; i < Equipment::Type::SIZE; i++) {
    Equipment::Type type = static_cast<Equipment::Type>(i);
    if (_equipmentSlots[type]) {
      if (!_isFacingRight && !_equipmentSprites[type]->isFlippedX()) {
        _equipmentSprites[type]->setFlippedX(true);
      } else if (_isFacingRight && _equipmentSprites[type]->isFlippedX()) {
        _equipmentSprites[type]->setFlippedX(false);
      }
      _equipmentSprites[type]->setPosition(b2bodyPos.x * kPpm, b2bodyPos.y * kPpm + 5);
    }
  }
}

void Character::reposition(float x, float y) {
  _b2body->SetTransform({x, y}, 0);
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

void Character::defineTexture(const string& spritesheetPath, float x, float y) {
  loadBodyAnimations(spritesheetPath);
  _bodySprite->setPosition(x * kPpm, y * kPpm + 7);

  runAnimation(State::IDLE_SHEATHED);
}

void Character::loadBodyAnimations(const string& bodySpritesheetPath) {
  _bodySpritesheet = SpriteBatchNode::create(bodySpritesheetPath + ".png");

  string prefix = asset_manager::getFrameNamePrefix(bodySpritesheetPath);
  _bodyAnimations[State::IDLE_SHEATHED] = createAnimation(prefix + "_idle_sheathed", 6, 10.0f / kPpm);
  _bodyAnimations[State::IDLE_UNSHEATHED] = createAnimation(prefix + "_idle_unsheathed", 6, 10.0f / kPpm);
  _bodyAnimations[State::RUNNING_SHEATHED] = createAnimation(prefix + "_running_sheathed", 8, 10.0f / kPpm);
  _bodyAnimations[State::RUNNING_UNSHEATHED] = createAnimation(prefix + "_running_unsheathed", 8, 10.0f / kPpm);
  _bodyAnimations[State::JUMPING_SHEATHED] = createAnimation(prefix + "_jumping_sheathed", 5, 10.0f / kPpm);
  _bodyAnimations[State::JUMPING_UNSHEATHED] = createAnimation(prefix + "_jumping_unsheathed", 5, 10.0f / kPpm);
  _bodyAnimations[State::FALLING_SHEATHED] = createAnimation(prefix + "_falling_sheathed", 1, 10.0f / kPpm);
  _bodyAnimations[State::FALLING_UNSHEATHED] = createAnimation(prefix + "_falling_unsheathed", 1, 10.0f / kPpm);
  _bodyAnimations[State::CROUCHING_SHEATHED] = createAnimation(prefix + "_crouching_sheathed", 2, 10.0f / kPpm);
  _bodyAnimations[State::CROUCHING_UNSHEATHED] = createAnimation(prefix + "_crouching_unsheathed", 2, 10.0f / kPpm);
  _bodyAnimations[State::SHEATHING_WEAPON] = createAnimation(prefix + "_sheathing_weapon", 6, 15.0f / kPpm);
  _bodyAnimations[State::UNSHEATHING_WEAPON] = createAnimation(prefix + "_unsheathing_weapon", 6, 15.0f / kPpm);
  _bodyAnimations[State::ATTACKING] = createAnimation(prefix + "_attacking", 8, 5.0f / kPpm);
  _bodyAnimations[State::KILLED] = createAnimation(prefix + "_killed", 6, 24.0f / kPpm);

  // Select a frame as default look for this sprite.
  _bodySprite = Sprite::createWithSpriteFrameName(prefix + "_idle_sheathed/0.png");
  _bodySprite->setScale(1.3f);

  _bodySpritesheet->addChild(_bodySprite);
  _bodySpritesheet->getTexture()->setAliasTexParameters(); // disable texture antialiasing
}

void Character::loadEquipmentAnimations(Equipment* equipment) {
  Equipment::Type type = equipment->getEquipmentType();
  _equipmentSpritesheets[type] = SpriteBatchNode::create(equipment->getSpritesPath() + ".png");

  string prefix = asset_manager::getFrameNamePrefix(equipment->getSpritesPath());
  _equipmentAnimations[type][State::IDLE_SHEATHED] = createAnimation(prefix + "_idle_sheathed", 6, 10.0f / kPpm);
  _equipmentAnimations[type][State::IDLE_UNSHEATHED] = createAnimation(prefix + "_idle_unsheathed", 6, 10.0f / kPpm);
  _equipmentAnimations[type][State::RUNNING_SHEATHED] = createAnimation(prefix + "_running_sheathed", 8, 10.0f / kPpm);
  _equipmentAnimations[type][State::RUNNING_UNSHEATHED] = createAnimation(prefix + "_running_unsheathed", 8, 10.0f / kPpm);
  _equipmentAnimations[type][State::JUMPING_SHEATHED] = createAnimation(prefix + "_jumping_sheathed", 5, 10.0f / kPpm);
  _equipmentAnimations[type][State::JUMPING_UNSHEATHED] = createAnimation(prefix + "_jumping_unsheathed", 5, 10.0f / kPpm);
  _equipmentAnimations[type][State::FALLING_SHEATHED] = createAnimation(prefix + "_falling_sheathed", 1, 10.0f / kPpm);
  _equipmentAnimations[type][State::FALLING_UNSHEATHED] = createAnimation(prefix + "_falling_unsheathed", 1, 10.0f / kPpm);
  _equipmentAnimations[type][State::CROUCHING_SHEATHED] = createAnimation(prefix + "_crouching_sheathed", 2, 10.0f / kPpm);
  _equipmentAnimations[type][State::CROUCHING_UNSHEATHED] = createAnimation(prefix + "_crouching_unsheathed", 2, 10.0f / kPpm);
  _equipmentAnimations[type][State::SHEATHING_WEAPON] = createAnimation(prefix + "_sheathing_weapon", 6, 15.0f / kPpm);
  _equipmentAnimations[type][State::UNSHEATHING_WEAPON] = createAnimation(prefix + "_unsheathing_weapon", 6, 15.0f / kPpm);
  _equipmentAnimations[type][State::ATTACKING] = createAnimation(prefix + "_attacking", 8, 5.0f / kPpm);
  _equipmentAnimations[type][State::KILLED] = createAnimation(prefix + "_killed", 6, 24.0f / kPpm);

  _equipmentSprites[type] = Sprite::createWithSpriteFrameName(prefix + "_idle_sheathed/0.png");
  _equipmentSprites[type]->setScale(1.3f);

  _equipmentSpritesheets[type]->addChild(_equipmentSprites[type]);
  _equipmentSpritesheets[type]->getTexture()->setAliasTexParameters();
}

Animation* Character::createAnimation(const string& frameName, size_t frameCount ,float delay) const {
  SpriteFrameCache* frameCache = SpriteFrameCache::getInstance();
  
  Vector<SpriteFrame*> frames;
  for (size_t i = 0; i < frameCount; i++) {
    const string& name = frameName + "/" + std::to_string(i) + ".png";
    frames.pushBack(frameCache->getSpriteFrameByName(name));
  }
  Animation* animation = Animation::createWithSpriteFrames(frames, delay);
  animation->retain();
  return animation;
}


void Character::runAnimation(State state, bool loop) const {
  // Update body animation.
  _bodySprite->stopAllActions();

  auto animation = Animate::create(_bodyAnimations[state]);
  Action* action = nullptr;
  if (loop) {
    action = RepeatForever::create(animation);
  } else {
    action = Repeat::create(animation, 1);
  }
  _bodySprite->runAction(action);

  // Update equipment animation.
  for (int i = 0; i < Equipment::Type::SIZE; i++) {
    Equipment::Type type = static_cast<Equipment::Type>(i);
    if (_equipmentSlots[type]) {
      _equipmentSprites[type]->stopAllActions();

      auto animation = Animate::create(_equipmentAnimations[type][state]);
      Action* action = nullptr;
      if (loop) {
        action = RepeatForever::create(animation);
      } else {
        action = Repeat::create(animation, 1);
      }
      _equipmentSprites[type]->runAction(action);
    }
  }
}

void Character::runAnimation(State state, const function<void ()>& func) const {
  _bodySprite->stopAllActions();

  auto animation = Animate::create(_bodyAnimations[state]);
  auto callback = CallFunc::create(func);
  _bodySprite->runAction(Sequence::createWithTwoActions(animation, callback));
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

    callback_util::runAfter([=]() {
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

  callback_util::runAfter([=]() {
    _isSheathingWeapon = false;
    _isWeaponSheathed = true;
  }, .8f);
}

void Character::unsheathWeapon() {
  _isUnsheathingWeapon = true;

  callback_util::runAfter([=]() {
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

  callback_util::runAfter([=]() {
    _isAttacking = false;
  }, .5f);

  if (!_inRangeTargets.empty()) {
    _lockedOnTarget = *_inRangeTargets.begin();

    if (!_lockedOnTarget->isInvincible()) {
      _lockedOnTarget->setAlerted(true);
      _lockedOnTarget->setLockedOnTarget(this);

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
  FloatingDamageManager::getInstance()->show(this, damage);

  if (_health <= 0) {
    source->getInRangeTargets().erase(this);
    Character::setCategoryBits(_bodyFixture, category_bits::kDestroyed);
    _isSetToKill = true;
    // TODO: play killed sound.
  } else {
    // TODO: play hurt sound.
  }
}


void Character::pickupItem(Item* item) {
  _inRangeItems.erase(item);
  addItem(item);
  item->getB2Body()->GetWorld()->DestroyBody(item->getB2Body());
}

void Character::addItem(Item* item) {
  _inventory[item->getItemType()].push_back(item);
}

void Character::removeItem(Item* item) {
  vector<Item*>& items = _inventory[item->getItemType()];
  items.erase(std::remove(items.begin(), items.end(), item), items.end());
}

void Character::equip(Equipment* equipment) {
  // If there's already an equipment in that slot, unequip it first.
  Equipment::Type type = equipment->getEquipmentType();
  if (_equipmentSlots[type]) {
    unequip(type);
  }
  removeItem(equipment);
  _equipmentSlots[type] = equipment;

  // Load equipment animations.
  loadEquipmentAnimations(equipment);
  GameMapManager::getInstance()->getLayer()->addChild(_equipmentSpritesheets[type]);
}

void Character::unequip(Equipment::Type equipmentType) {
  // If there's an equipped item in the target slot,
  // move it into character's inventory.
  if (_equipmentSlots[equipmentType]) {
    Equipment* e = _equipmentSlots[equipmentType];
    _equipmentSlots[equipmentType] = nullptr;
    addItem(e);

    GameMapManager::getInstance()->getLayer()->removeChild(_equipmentSpritesheets[equipmentType]);
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


void Character::setJumping(bool jumping) {
  _isJumping = jumping;
}

void Character::setOnPlatform(bool onPlatform) {
  _isOnPlatform = onPlatform;
}

void Character::setAttacking(bool attacking) {
  _isAttacking = attacking;
}

void Character::setCrouching(bool crouching) {
  _isCrouching = crouching;
}

void Character::setInvincible(bool invincible) {
  _isInvincible = invincible;
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

void Character::setAlerted(bool alerted) {
  _isAlerted = alerted;
}


set<Item*>& Character::getInRangeItems() {
  return _inRangeItems;
}


const Character::Inventory& Character::getInventory() const {
  return _inventory;
}

const Character::EquipmentSlots& Character::getEquipmentSlots() const {
  return _equipmentSlots;
}

Portal* Character::getPortal() const {
  return _portal;
}

void Character::setPortal(Portal* portal) {
  _portal = portal;
}


b2Body* Character::getB2Body() const {
  return _b2body;
}

Sprite* Character::getBodySprite() const {
  return _bodySprite;
}

SpriteBatchNode* Character::getBodySpritesheet() const {
  return _bodySpritesheet;
}


void Character::setCategoryBits(b2Fixture* fixture, short bits) {
  b2Filter filter;
  filter.categoryBits = bits;
  fixture->SetFilterData(filter);
}

} // namespace vigilante
