#include "Character.h"

#include "GameAssetManager.h"
#include "character/data/CharacterProfile.h"
#include "map/GameMapManager.h"
#include "ui/damage/FloatingDamageManager.h"
#include "ui/notification/NotificationManager.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/CallbackUtil.h"
#include "util/CategoryBits.h"
#include "util/Constants.h"

using std::set;
using std::array;
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
using cocos2d::FileUtils;

namespace vigilante {

const array<string, Character::State::SIZE> Character::_kCharacterStateStr = {{
  "idle_sheathed",
  "idle_unsheathed",
  "running_sheathed",
  "running_unsheathed",
  "jumping_sheathed",
  "jumping_unsheathed",
  "falling_sheathed",
  "falling_unsheathed",
  "crouching_sheathed",
  "crouching_unsheathed",
  "sheathing_weapon",
  "unsheathing_weapon",
  "attacking",
  "killed"
}};

Character::Character(float x, float y)
    : Actor(),
      _currentState(State::IDLE_SHEATHED),
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
      _profile(),
      _lockedOnTarget(),
      _isAlerted(),
      _inventory(),
      _equipmentSlots(),
      _portal() {
  _fixtures.resize(3);
  _bodyAnimations.resize(State::SIZE);
}

Character::~Character() {
  if (!_isKilled) {
    _body->GetWorld()->DestroyBody(_body);
  }

  // Delete all items from inventory and equipment slots.
  for (auto& items : _inventory) { // vector<Item*>
    for (const auto item : items) { // Item*
      delete item;
    }
  }
  for (auto equipment : _equipmentSlots) {
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
          GameMapManager::getInstance()->getWorld()->DestroyBody(_body);
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
    b2CircleShape* shape = static_cast<b2CircleShape*>(_fixtures[Fixture::WEAPON]->GetShape());
    shape->m_p = {-_profile.attackRange / kPpm, 0};
  } else if (_isFacingRight && _bodySprite->isFlippedX()) {
    _bodySprite->setFlippedX(false);
    b2CircleShape* shape = static_cast<b2CircleShape*>(_fixtures[Fixture::WEAPON]->GetShape());
    shape->m_p = {_profile.attackRange / kPpm, 0};
  }

  // Sync the body sprite with its b2body.
  b2Vec2 b2bodyPos = _body->GetPosition();
  _bodySprite->setPosition(b2bodyPos.x * kPpm, b2bodyPos.y * kPpm + _profile.spriteOffsetY);

  // Sync the equipment sprites with its b2body.
  for (int i = 0; i < Equipment::Type::SIZE; i++) {
    Equipment::Type type = static_cast<Equipment::Type>(i);
    if (_equipmentSlots[type]) {
      if (!_isFacingRight && !_equipmentSprites[type]->isFlippedX()) {
        _equipmentSprites[type]->setFlippedX(true);
      } else if (_isFacingRight && _equipmentSprites[type]->isFlippedX()) {
        _equipmentSprites[type]->setFlippedX(false);
      }
      _equipmentSprites[type]->setPosition(b2bodyPos.x * kPpm, b2bodyPos.y * kPpm + _profile.spriteOffsetY);
    }
  }
}

void Character::setPosition(float x, float y) {
  _body->SetTransform({x, y}, 0);
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

  _body = bodyBuilder.type(bodyType)
    .position(x, y, kPpm)
    .buildBody();

  // Create body fixture.
  // Fixture position in box2d is relative to b2body's position.
  float scaleFactor = Director::getInstance()->getContentScaleFactor();
  b2Vec2 vertices[4];
  float bw = _profile.bodyWidth;
  float bh = _profile.bodyHeight;
  vertices[0] = {-bw / 2 / scaleFactor,  bh / 2 / scaleFactor};
  vertices[1] = { bw / 2 / scaleFactor,  bh / 2 / scaleFactor};
  vertices[2] = {-bw / 2 / scaleFactor, -bh / 2 / scaleFactor};
  vertices[3] = { bw / 2 / scaleFactor, -bh / 2 / scaleFactor};

  _fixtures[Fixture::BODY] = bodyBuilder.newPolygonFixture(vertices, 4, kPpm)
    .categoryBits(bodyCategoryBits)
    .maskBits(bodyMaskBits)
    .setUserData(this)
    .buildFixture();


  // Create feet fixture.
  b2Vec2 feetVertices[4];
  feetVertices[0] = {(-bw / 2 + 1) / scaleFactor, 0};
  feetVertices[1] = {( bw / 2 - 1) / scaleFactor, 0};
  feetVertices[2] = {(-bw / 2 + 1) / scaleFactor, (-bh / 2 - 1) / scaleFactor};
  feetVertices[3] = {( bw / 2 - 1) / scaleFactor, (-bh / 2 - 1) / scaleFactor};

  _fixtures[Fixture::FEET] = bodyBuilder.newPolygonFixture(feetVertices, 4, kPpm)
    .categoryBits(category_bits::kFeet)
    .maskBits(feetMaskBits)
    .setUserData(this)
    .buildFixture();


  // Create weapon fixture.
  float atkRange = _profile.attackRange;
  _fixtures[Fixture::WEAPON] = bodyBuilder.newCircleFixture({atkRange, 0}, atkRange, kPpm)
    .categoryBits(category_bits::kMeleeWeapon)
    .maskBits(weaponMaskBits)
    .setSensor(true)
    .setUserData(this)
    .buildFixture();
}

void Character::defineTexture(const string& bodyTextureResPath, float x, float y) {
  loadBodyAnimations(bodyTextureResPath);
  _bodySprite->setPosition(x * kPpm, y * kPpm + _profile.spriteOffsetY);

  runAnimation(State::IDLE_SHEATHED);
}

void Character::loadBodyAnimations(const string& bodyTextureResPath) {
  _bodySpritesheet = SpriteBatchNode::create(bodyTextureResPath + "/spritesheet.png");

  string framePrefix = asset_manager::getFrameNamePrefix(bodyTextureResPath);
  _bodyAnimations[State::IDLE_SHEATHED] = createAnimation(bodyTextureResPath, _kCharacterStateStr[State::IDLE_SHEATHED], _profile.frameInterval[State::IDLE_SHEATHED] / kPpm);
  Animation* fallback = _bodyAnimations[State::IDLE_SHEATHED];
  _bodyAnimations[State::IDLE_UNSHEATHED] = createAnimation(bodyTextureResPath, _kCharacterStateStr[State::IDLE_UNSHEATHED], _profile.frameInterval[State::IDLE_UNSHEATHED] / kPpm, fallback);
  _bodyAnimations[State::RUNNING_SHEATHED] = createAnimation(bodyTextureResPath, _kCharacterStateStr[State::RUNNING_SHEATHED], _profile.frameInterval[State::RUNNING_SHEATHED] / kPpm, fallback);
  _bodyAnimations[State::RUNNING_UNSHEATHED] = createAnimation(bodyTextureResPath, _kCharacterStateStr[State::RUNNING_UNSHEATHED], _profile.frameInterval[State::RUNNING_UNSHEATHED] / kPpm, fallback);
  _bodyAnimations[State::JUMPING_SHEATHED] = createAnimation(bodyTextureResPath, _kCharacterStateStr[State::JUMPING_SHEATHED], _profile.frameInterval[State::JUMPING_SHEATHED] / kPpm, fallback);
  _bodyAnimations[State::JUMPING_UNSHEATHED] = createAnimation(bodyTextureResPath, _kCharacterStateStr[State::JUMPING_UNSHEATHED], _profile.frameInterval[State::JUMPING_UNSHEATHED] / kPpm, fallback);
  _bodyAnimations[State::FALLING_SHEATHED] = createAnimation(bodyTextureResPath, _kCharacterStateStr[State::FALLING_SHEATHED], _profile.frameInterval[State::FALLING_SHEATHED] / kPpm, fallback);
  _bodyAnimations[State::FALLING_UNSHEATHED] = createAnimation(bodyTextureResPath, _kCharacterStateStr[State::FALLING_UNSHEATHED], _profile.frameInterval[State::FALLING_UNSHEATHED] / kPpm, fallback);
  _bodyAnimations[State::CROUCHING_SHEATHED] = createAnimation(bodyTextureResPath, _kCharacterStateStr[State::CROUCHING_SHEATHED], _profile.frameInterval[State::CROUCHING_SHEATHED] / kPpm, fallback);
  _bodyAnimations[State::CROUCHING_UNSHEATHED] = createAnimation(bodyTextureResPath, _kCharacterStateStr[State::CROUCHING_UNSHEATHED], _profile.frameInterval[State::CROUCHING_UNSHEATHED] / kPpm, fallback);
  _bodyAnimations[State::SHEATHING_WEAPON] = createAnimation(bodyTextureResPath, _kCharacterStateStr[State::SHEATHING_WEAPON], _profile.frameInterval[State::SHEATHING_WEAPON] / kPpm, fallback);
  _bodyAnimations[State::UNSHEATHING_WEAPON] = createAnimation(bodyTextureResPath, _kCharacterStateStr[State::UNSHEATHING_WEAPON], _profile.frameInterval[State::UNSHEATHING_WEAPON] / kPpm, fallback);
  _bodyAnimations[State::ATTACKING] = createAnimation(bodyTextureResPath, _kCharacterStateStr[State::ATTACKING], _profile.frameInterval[State::ATTACKING] / kPpm, fallback);
  _bodyAnimations[State::KILLED] = createAnimation(bodyTextureResPath, _kCharacterStateStr[State::KILLED], _profile.frameInterval[State::KILLED] / kPpm, fallback);

  // Select a frame as default look for this sprite.
  _bodySprite = Sprite::createWithSpriteFrameName(framePrefix + "_idle_sheathed/0.png");
  _bodySprite->setScaleX(_profile.spriteScaleX);
  _bodySprite->setScaleY(_profile.spriteScaleY);

  _bodySpritesheet->addChild(_bodySprite);
  _bodySpritesheet->getTexture()->setAliasTexParameters(); // disable texture antialiasing
}

void Character::loadEquipmentAnimations(Equipment* equipment) {
  Equipment::Type type = equipment->getEquipmentType();
  _equipmentSpritesheets[type] = SpriteBatchNode::create(equipment->getSpritesPath() + "/spritesheet.png");

  const string& textureResPath = equipment->getSpritesPath();
  _equipmentAnimations[type][State::IDLE_SHEATHED] = createAnimation(textureResPath, _kCharacterStateStr[State::IDLE_SHEATHED], _profile.frameInterval[State::IDLE_SHEATHED] / kPpm);
  Animation* fallback = _equipmentAnimations[type][State::IDLE_SHEATHED];
  _equipmentAnimations[type][State::IDLE_UNSHEATHED] = createAnimation(textureResPath, _kCharacterStateStr[State::IDLE_UNSHEATHED], _profile.frameInterval[State::IDLE_UNSHEATHED] / kPpm, fallback);
  _equipmentAnimations[type][State::RUNNING_SHEATHED] = createAnimation(textureResPath, _kCharacterStateStr[State::RUNNING_SHEATHED], _profile.frameInterval[State::RUNNING_SHEATHED] / kPpm, fallback);
  _equipmentAnimations[type][State::RUNNING_UNSHEATHED] = createAnimation(textureResPath, _kCharacterStateStr[State::RUNNING_UNSHEATHED], _profile.frameInterval[State::RUNNING_UNSHEATHED] / kPpm, fallback);
  _equipmentAnimations[type][State::JUMPING_SHEATHED] = createAnimation(textureResPath, _kCharacterStateStr[State::JUMPING_SHEATHED], _profile.frameInterval[State::JUMPING_SHEATHED] / kPpm, fallback);
  _equipmentAnimations[type][State::JUMPING_UNSHEATHED] = createAnimation(textureResPath, _kCharacterStateStr[State::JUMPING_UNSHEATHED], _profile.frameInterval[State::JUMPING_UNSHEATHED] / kPpm, fallback);
  _equipmentAnimations[type][State::FALLING_SHEATHED] = createAnimation(textureResPath, _kCharacterStateStr[State::FALLING_SHEATHED], _profile.frameInterval[State::FALLING_SHEATHED] / kPpm, fallback);
  _equipmentAnimations[type][State::FALLING_UNSHEATHED] = createAnimation(textureResPath, _kCharacterStateStr[State::FALLING_UNSHEATHED], _profile.frameInterval[State::FALLING_UNSHEATHED] / kPpm, fallback);
  _equipmentAnimations[type][State::CROUCHING_SHEATHED] = createAnimation(textureResPath, _kCharacterStateStr[State::CROUCHING_SHEATHED], _profile.frameInterval[State::CROUCHING_SHEATHED] / kPpm, fallback);
  _equipmentAnimations[type][State::CROUCHING_UNSHEATHED] = createAnimation(textureResPath, _kCharacterStateStr[State::CROUCHING_UNSHEATHED], _profile.frameInterval[State::CROUCHING_UNSHEATHED] / kPpm, fallback);
  _equipmentAnimations[type][State::SHEATHING_WEAPON] = createAnimation(textureResPath, _kCharacterStateStr[State::SHEATHING_WEAPON], _profile.frameInterval[State::SHEATHING_WEAPON] / kPpm, fallback);
  _equipmentAnimations[type][State::UNSHEATHING_WEAPON] = createAnimation(textureResPath, _kCharacterStateStr[State::UNSHEATHING_WEAPON], _profile.frameInterval[State::UNSHEATHING_WEAPON] / kPpm, fallback);
  _equipmentAnimations[type][State::ATTACKING] = createAnimation(textureResPath, _kCharacterStateStr[State::ATTACKING], _profile.frameInterval[State::ATTACKING] / kPpm, fallback);
  _equipmentAnimations[type][State::KILLED] = createAnimation(textureResPath, _kCharacterStateStr[State::KILLED], _profile.frameInterval[State::KILLED] / kPpm, fallback);

  string framePrefix = asset_manager::getFrameNamePrefix(equipment->getSpritesPath());
  _equipmentSprites[type] = Sprite::createWithSpriteFrameName(framePrefix + "_idle_sheathed/0.png");
  _equipmentSprites[type]->setScaleX(_profile.spriteScaleX);
  _equipmentSprites[type]->setScaleY(_profile.spriteScaleY);

  _equipmentSpritesheets[type]->addChild(_equipmentSprites[type]);
  _equipmentSpritesheets[type]->getTexture()->setAliasTexParameters();
}


void Character::runAnimation(State state, bool loop) const {
  auto animation = Animate::create(_bodyAnimations[state]);
  Action* action = nullptr;
  if (loop) {
    action = RepeatForever::create(animation);
  } else {
    action = Repeat::create(animation, 1);
  }

  // Update body animation.
  _bodySprite->stopAllActions();
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
  } else if (_body->GetLinearVelocity().y < -.5f) {
    return (_isWeaponSheathed) ? State::FALLING_SHEATHED : State::FALLING_UNSHEATHED;
  } else if (_isCrouching) {
    return (_isWeaponSheathed) ? State::CROUCHING_SHEATHED : State::CROUCHING_UNSHEATHED;
  } else if (std::abs(_body->GetLinearVelocity().x) > .01f) {
    return (_isWeaponSheathed) ? State::RUNNING_SHEATHED : State::RUNNING_UNSHEATHED;
  } else {
    return (_isWeaponSheathed) ? State::IDLE_SHEATHED : State::IDLE_UNSHEATHED;
  }
}


void Character::moveLeft() {
  _isFacingRight = false;
  if (_body->GetLinearVelocity().x >= -_profile.moveSpeed * 2) {
    _body->ApplyLinearImpulse({-_profile.moveSpeed, 0}, _body->GetWorldCenter(), true);
  }
}

void Character::moveRight() {
  _isFacingRight = true;
  if (_body->GetLinearVelocity().x <= _profile.moveSpeed * 2) {
    _body->ApplyLinearImpulse({_profile.moveSpeed, 0}, _body->GetWorldCenter(), true);
  }
}

void Character::jump() {
  if (!_isJumping) {
    _isJumping = true;
    _body->ApplyLinearImpulse({0, _profile.jumpHeight}, _body->GetWorldCenter(), true);
  }
}

void Character::jumpDown() {
  if (_isOnPlatform) {
    _fixtures[Fixture::FEET]->SetSensor(true);

    callback_util::runAfter([=]() {
      _fixtures[Fixture::FEET]->SetSensor(false);
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
  if (_isAttacking) {
    return;
  }
  if (_isWeaponSheathed) {
    unsheathWeapon();
    return;
  }

  _isAttacking = true;

  callback_util::runAfter([=]() {
    _isAttacking = false;
  }, _profile.attackTime);

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
  b2Body* b2body = target->getBody();
  b2body->ApplyLinearImpulse({forceX, forceY}, b2body->GetWorldCenter(), true);
}

void Character::inflictDamage(Character* target, int damage) {
  target->receiveDamage(this, damage);
}

void Character::receiveDamage(Character* source, int damage) {
  if (_isInvincible) {
    return;
  }

  _profile.health -= damage;
  FloatingDamageManager::getInstance()->show(this, damage);

  if (_profile.health <= 0) {
    source->getInRangeTargets().erase(this);
    Character::setCategoryBits(_fixtures[Fixture::BODY], category_bits::kDestroyed);
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
  GameMapManager::getInstance()->getLayer()->addChild(_equipmentSpritesheets[type], 33);
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


CharacterProfile& Character::getProfile() {
  return _profile;
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

GameMap::Portal* Character::getPortal() const {
  return _portal;
}

void Character::setPortal(GameMap::Portal* portal) {
  _portal = portal;
}


void Character::setCategoryBits(b2Fixture* fixture, short bits) {
  b2Filter filter;
  filter.categoryBits = bits;
  fixture->SetFilterData(filter);
}

} // namespace vigilante
