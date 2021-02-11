// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Character.h"

#include <algorithm>
#include <cassert>

#include <json/document.h>
#include "AssetManager.h"
#include "CallbackManager.h"
#include "Constants.h"
#include "Player.h"
#include "gameplay/ExpPointTable.h"
#include "map/GameMapManager.h"
#include "ui/hud/Hud.h"
#include "ui/floating_damages/FloatingDamages.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/RandUtil.h"
#include "util/JsonUtil.h"
#include "util/Logger.h"

using std::set;
using std::array;
using std::unordered_set;
using std::string;
using std::function;
using std::unique_ptr;
using std::shared_ptr;
using cocos2d::Director;
using cocos2d::Sequence;
using cocos2d::CallFunc;
using cocos2d::Repeat;
using cocos2d::RepeatForever;
using cocos2d::Animation;
using cocos2d::Animate;
using cocos2d::Action;
using cocos2d::Sprite;
using cocos2d::FileUtils;
using cocos2d::SpriteBatchNode;
using rapidjson::Document;

namespace vigilante {

const array<string, Character::State::STATE_SIZE> Character::_kCharacterStateStr = {{
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
  "attacking0",
  "killed"
}};

Character::Character(const string& jsonFileName)
    : DynamicActor(State::STATE_SIZE, FixtureType::FIXTURE_SIZE),
      _characterProfile(jsonFileName),
      _statsRegenTimer(),
      _baseRegenDeltaHealth(5),
      _baseRegenDeltaMagicka(5),
      _baseRegenDeltaStamina(5),
      _currentState(State::IDLE_SHEATHED),
      _previousState(State::IDLE_SHEATHED),
      _isFacingRight(true),
      _isWeaponSheathed(true),
      _isSheathingWeapon(),
      _isUnsheathingWeapon(),
      _isJumping(),
      _isDoubleJumping(),
      _isOnPlatform(),
      _isAttacking(),
      _isUsingSkill(),
      _isCrouching(),
      _isInvincible(),
      _isTakingDamage(),
      _isKilled(),
      _isSetToKill(),
      _inRangeTargets(),
      _lockedOnTarget(),
      _isAlerted(),
      _inventory(),
      _equipmentSlots(),
      _itemMapper(),
      _interactableObject(),
      _portal(),
      _skillBook(),
      _skillMapper(),
      _currentlyUsedSkill(),
      // There will be at least `1` attack animation.
      _kAttackAnimationIdxMax(1 + getExtraAttackAnimationsCount()),
      _attackAnimationIdx(),
      _bodyExtraAttackAnimations(_kAttackAnimationIdxMax - 1),
      _equipmentExtraAttackAnimations(),
      _equipmentSprites(),
      _equipmentSpritesheets(),
      _equipmentAnimations(),
      _skillBodyAnimations(),
      _party() {
  // Resize each vector in _equipmentExtraAttackAnimations to match
  // the size of _bodyExtraAttackAnimations.
  for (auto& animationVector : _equipmentExtraAttackAnimations) {
    animationVector.resize(_bodyExtraAttackAnimations.size());
  }

  // Populate this character's _skillBook with the skills it knows by default.
  for (const auto& s : _characterProfile.defaultSkills) {
    addSkill(Skill::create(s, this));
  }
  // Popuplate this character's _inventory with the items it owns by default.
  for (const auto& p : _characterProfile.defaultInventory) {
    addItem(Item::create(p.first), p.second);
  }
}


void Character::removeFromMap() {
  if (!_isShownOnMap) {
    return;
  }
  _isShownOnMap = false;

  if (!_isKilled) {
    _body->GetWorld()->DestroyBody(_body);
  }

  // Remove _bodySpritesheet
  GameMapManager::getInstance()->getLayer()->removeChild(_bodySpritesheet);
  // Remove _equipmentSpritesheets
  for (auto equipment : _equipmentSlots) {
    if (equipment) {
      GameMapManager::getInstance()->getLayer()->removeChild(
          _equipmentSpritesheets[equipment->getEquipmentProfile().equipmentType]
      );
    }
  }
}

void Character::update(float delta) {
  if (!_isShownOnMap || _isKilled) {
    return;
  }

  // Flip the sprite if needed.
  if (!_isFacingRight && !_bodySprite->isFlippedX()) {
    _bodySprite->setFlippedX(true);
    auto shape = dynamic_cast<b2CircleShape*>(_fixtures[FixtureType::WEAPON]->GetShape());
    shape->m_p = {-_characterProfile.attackRange / kPpm, 0};
  } else if (_isFacingRight && _bodySprite->isFlippedX()) {
    _bodySprite->setFlippedX(false);
    auto shape = dynamic_cast<b2CircleShape*>(_fixtures[FixtureType::WEAPON]->GetShape());
    shape->m_p = {_characterProfile.attackRange / kPpm, 0};
  }

  const b2Vec2& b2bodyPos = _body->GetPosition();

  // Sync the body sprite with its b2body.
   _bodySprite->setPosition(_body->GetPosition().x * kPpm + _characterProfile.spriteOffsetX,
                            _body->GetPosition().y * kPpm + _characterProfile.spriteOffsetY);

  // Sync the equipment sprites with its b2body.
  for (int type = 0; type < static_cast<int>(Equipment::Type::SIZE); type++) {
    if (!_equipmentSlots[type]) {
      continue;
    }

    if (!_isFacingRight && !_equipmentSprites[type]->isFlippedX()) {
      _equipmentSprites[type]->setFlippedX(true);
    } else if (_isFacingRight && _equipmentSprites[type]->isFlippedX()) {
      _equipmentSprites[type]->setFlippedX(false);
    }
    _equipmentSprites[type]->setPosition(b2bodyPos.x * kPpm + _characterProfile.spriteOffsetX,
                                         b2bodyPos.y * kPpm + _characterProfile.spriteOffsetY);
  }

  // Handle stats regeneration.
  _statsRegenTimer += delta;
  if (_statsRegenTimer >= 5.0f) {
    _statsRegenTimer = 0;
    regenHealth(_baseRegenDeltaHealth);
    regenMagicka(_baseRegenDeltaMagicka);
    regenStamina(_baseRegenDeltaStamina);
    Hud::getInstance()->updateStatusBars();
  }

  // Don't update character's state if he/she is using skill.
  if (_isUsingSkill) {
    return;
  }

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
        // `onKilled()` will be executed after the KILLED animation
        // has finished.
        runAnimation(State::KILLED, [this]() {
          onKilled();
        });
        break;
      case State::IDLE_SHEATHED:
        runAnimation(State::IDLE_SHEATHED, true);
        break;
      case State::IDLE_UNSHEATHED:  // fallthrough
      default:
        runAnimation(State::IDLE_UNSHEATHED, true);
        break;
    }
  }
}

void Character::import(const string& jsonFileName) {
  _characterProfile = Character::Profile(jsonFileName);
}


void Character::defineBody(b2BodyType bodyType, float x, float y,
                           short bodyCategoryBits, short bodyMaskBits,
                           short feetMaskBits, short weaponMaskBits) {
  b2World* world = GameMapManager::getInstance()->getWorld();
  b2BodyBuilder bodyBuilder(world);

  _body = bodyBuilder.type(bodyType)
    .position(x, y, kPpm)
    .buildBody();

  // Create body fixture.
  // Fixture position in box2d is relative to b2body's position.
  float scaleFactor = Director::getInstance()->getContentScaleFactor();
  b2Vec2 vertices[4];
  float bw = _characterProfile.bodyWidth;
  float bh = _characterProfile.bodyHeight;
  vertices[0] = {-bw / 2 / scaleFactor,  bh / 2 / scaleFactor};
  vertices[1] = { bw / 2 / scaleFactor,  bh / 2 / scaleFactor};
  vertices[2] = {-bw / 2 / scaleFactor, -bh / 2 / scaleFactor};
  vertices[3] = { bw / 2 / scaleFactor, -bh / 2 / scaleFactor};

  _fixtures[FixtureType::BODY] = bodyBuilder.newPolygonFixture(vertices, 4, kPpm)
    .categoryBits(bodyCategoryBits)
    .maskBits(bodyMaskBits)
    .setSensor(true)
    .setUserData(this)
    .buildFixture();


  // Create feet fixture.
  b2Vec2 feetVertices[4];
  feetVertices[0] = {(-bw / 2 + 1) / scaleFactor, 0};
  feetVertices[1] = {( bw / 2 - 1) / scaleFactor, 0};
  feetVertices[2] = {(-bw / 2 + 1) / scaleFactor, (-bh / 2 - 1) / scaleFactor};
  feetVertices[3] = {( bw / 2 - 1) / scaleFactor, (-bh / 2 - 1) / scaleFactor};

  _fixtures[FixtureType::FEET] = bodyBuilder.newPolygonFixture(feetVertices, 4, kPpm)
    .categoryBits(category_bits::kFeet)
    .maskBits(feetMaskBits)
    .setUserData(this)
    .buildFixture();


  // Create weapon fixture.
  float atkRange = _characterProfile.attackRange;
  _fixtures[FixtureType::WEAPON] = bodyBuilder.newCircleFixture({atkRange, 0}, atkRange, kPpm)
    .categoryBits(category_bits::kMeleeWeapon)
    .maskBits(weaponMaskBits)
    .setSensor(true)
    .setUserData(this)
    .buildFixture();
}

void Character::defineTexture(const string& bodyTextureResDir, float x, float y) {
  loadBodyAnimations(bodyTextureResDir);
  _bodySprite->setPosition(x * kPpm, y * kPpm + _characterProfile.spriteOffsetY);

  runAnimation(State::IDLE_SHEATHED);
}

void Character::loadBodyAnimations(const string& bodyTextureResDir) {
#define CREATE_BODY_ANIMATION(state, fallback)         \
  do {                                                 \
    _bodyAnimations[state] = createAnimation(          \
        _characterProfile.textureResDir,               \
        _kCharacterStateStr[state],                    \
        _characterProfile.frameInterval[state] / kPpm, \
        fallback                                       \
    );                                                 \
  } while (0)

  CREATE_BODY_ANIMATION(State::IDLE_SHEATHED, nullptr);
  Animation* fallback = _bodyAnimations[State::IDLE_SHEATHED];
  CREATE_BODY_ANIMATION(State::IDLE_UNSHEATHED, fallback);
  CREATE_BODY_ANIMATION(State::RUNNING_SHEATHED, fallback);
  CREATE_BODY_ANIMATION(State::RUNNING_UNSHEATHED, fallback);
  CREATE_BODY_ANIMATION(State::JUMPING_SHEATHED, fallback);
  CREATE_BODY_ANIMATION(State::JUMPING_UNSHEATHED, fallback);
  CREATE_BODY_ANIMATION(State::FALLING_SHEATHED, fallback);
  CREATE_BODY_ANIMATION(State::FALLING_UNSHEATHED, fallback);
  CREATE_BODY_ANIMATION(State::CROUCHING_SHEATHED, fallback);
  CREATE_BODY_ANIMATION(State::CROUCHING_UNSHEATHED, fallback);
  CREATE_BODY_ANIMATION(State::SHEATHING_WEAPON, fallback);
  CREATE_BODY_ANIMATION(State::UNSHEATHING_WEAPON, fallback);
  CREATE_BODY_ANIMATION(State::ATTACKING, fallback);
  CREATE_BODY_ANIMATION(State::KILLED, fallback);

  // Load extra attack animations.
  for (size_t i = 0; i < _bodyExtraAttackAnimations.size(); i++) {
    _bodyExtraAttackAnimations[i] = createAnimation(
        bodyTextureResDir,
        "attacking" + std::to_string(1 + i),
        _characterProfile.frameInterval[State::ATTACKING] / kPpm,
        fallback
    );
  }
  
  // Select a frame as default look for this sprite.
  string framePrefix = StaticActor::getLastDirName(bodyTextureResDir);
  _bodySprite = Sprite::createWithSpriteFrameName(framePrefix + "_idle_sheathed/0.png");
  _bodySprite->setScale(_characterProfile.spriteScaleX,
                        _characterProfile.spriteScaleY);

  _bodySpritesheet = SpriteBatchNode::create(bodyTextureResDir + "/spritesheet.png");
  _bodySpritesheet->getTexture()->setAliasTexParameters();  // disable texture antialiasing
  _bodySpritesheet->addChild(_bodySprite);
}

void Character::loadEquipmentAnimations(Equipment* equipment) {
#define CREATE_EQUIPMENT_ANIMATION(equipment, state, fallback)                                     \
  do {                                                                                             \
    _equipmentAnimations[equipment->getEquipmentProfile().equipmentType][state] = createAnimation( \
        equipment->getItemProfile().textureResDir,                                                 \
        _kCharacterStateStr[state],                                                                \
        _characterProfile.frameInterval[state] / kPpm,                                             \
        fallback                                                                                   \
    );                                                                                             \
  } while (0)

  Equipment::Type type = equipment->getEquipmentProfile().equipmentType;
  const string& textureResDir = equipment->getItemProfile().textureResDir;
  
  CREATE_EQUIPMENT_ANIMATION(equipment, State::IDLE_SHEATHED, nullptr);
  Animation* fallback = _equipmentAnimations[type][State::IDLE_SHEATHED];
  CREATE_EQUIPMENT_ANIMATION(equipment, State::IDLE_UNSHEATHED, fallback);
  CREATE_EQUIPMENT_ANIMATION(equipment, State::RUNNING_SHEATHED, fallback);
  CREATE_EQUIPMENT_ANIMATION(equipment, State::RUNNING_UNSHEATHED, fallback);
  CREATE_EQUIPMENT_ANIMATION(equipment, State::JUMPING_SHEATHED, fallback);
  CREATE_EQUIPMENT_ANIMATION(equipment, State::JUMPING_UNSHEATHED, fallback);
  CREATE_EQUIPMENT_ANIMATION(equipment, State::FALLING_SHEATHED, fallback);
  CREATE_EQUIPMENT_ANIMATION(equipment, State::FALLING_UNSHEATHED, fallback);
  CREATE_EQUIPMENT_ANIMATION(equipment, State::CROUCHING_SHEATHED, fallback);
  CREATE_EQUIPMENT_ANIMATION(equipment, State::CROUCHING_UNSHEATHED, fallback);
  CREATE_EQUIPMENT_ANIMATION(equipment, State::SHEATHING_WEAPON, fallback);
  CREATE_EQUIPMENT_ANIMATION(equipment, State::UNSHEATHING_WEAPON, fallback);
  CREATE_EQUIPMENT_ANIMATION(equipment, State::ATTACKING, fallback);
  CREATE_EQUIPMENT_ANIMATION(equipment, State::KILLED, fallback);

  // Load extra attack animations.
  for (size_t i = 0; i < _bodyExtraAttackAnimations.size(); i++) {
    _equipmentExtraAttackAnimations[type][i] = createAnimation(
        textureResDir,
        "attacking" + std::to_string(1 + i),
        _characterProfile.frameInterval[State::ATTACKING] / kPpm,
        fallback
    );
  }

  // Select a frame as default look for this sprite.
  string framePrefix = StaticActor::getLastDirName(textureResDir);
  _equipmentSprites[type] = Sprite::createWithSpriteFrameName(framePrefix + "_idle_sheathed/0.png");
  _equipmentSprites[type]->setScale(_characterProfile.spriteScaleX,
                                    _characterProfile.spriteScaleY);

  _equipmentSpritesheets[type] = SpriteBatchNode::create(textureResDir + "/spritesheet.png");
  _equipmentSpritesheets[type]->getTexture()->setAliasTexParameters();
  _equipmentSpritesheets[type]->addChild(_equipmentSprites[type]);
}

int Character::getExtraAttackAnimationsCount() const {
  FileUtils* fileUtils = FileUtils::getInstance();
  const string& framesNamePrefix = StaticActor::getLastDirName(_characterProfile.textureResDir);

  // player_attacking0  // must have!
  // player_attacking1  // optional...
  // player_attacking2  // optional...
  // ...
  string dir = _characterProfile.textureResDir + "/" + framesNamePrefix + "_" + "attacking";
  int frameCount = 0;
  fileUtils->setPopupNotify(false);  // disable CCLOG
  while (fileUtils->isDirectoryExist(dir + std::to_string(frameCount + 1))) {
    frameCount++;
  }
  fileUtils->setPopupNotify(true);

  return frameCount;
}

Animation* Character::getBodyAttackAnimation() const {
  return (_attackAnimationIdx == 0) ? _bodyAnimations[State::ATTACKING] :
                                      _bodyExtraAttackAnimations[_attackAnimationIdx - 1];
}

Animation* Character::getEquipmentAttackAnimation(const Equipment::Type type) const {
  return (_attackAnimationIdx == 0) ? _equipmentAnimations[type][State::ATTACKING] :
                                      _equipmentExtraAttackAnimations[type][_attackAnimationIdx - 1];
}


void Character::runAnimation(State state, bool loop) {
  // Update body animation.
  Animate* animate = Animate::create((state != State::ATTACKING) ? _bodyAnimations[state] :
                                                                   getBodyAttackAnimation());
  _bodySprite->stopAllActions();
  _bodySprite->runAction((loop) ? dynamic_cast<Action*>(RepeatForever::create(animate)) :
                                  dynamic_cast<Action*>(Repeat::create(animate, 1)));

  // Update equipment animation.
  for (int type = 0; type < static_cast<int>(Equipment::Type::SIZE); type++) {
    if (!_equipmentSlots[type]) {
      continue;
    }

    Animate* animate = Animate::create((state != State::ATTACKING) ?
        _equipmentAnimations[type][state] :
        getEquipmentAttackAnimation(static_cast<Equipment::Type>(type)));

    _equipmentSprites[type]->stopAllActions();
    _equipmentSprites[type]->runAction((loop) ? dynamic_cast<Action*>(RepeatForever::create(animate)) :
                                                dynamic_cast<Action*>(Repeat::create(animate, 1)));
  }

  // If `state` is ATTACKING, then increment `_attackAnimationIdx` 
  // and wrap around when needed.
  if (state == State::ATTACKING) {
    _attackAnimationIdx = (_attackAnimationIdx + 1) % _kAttackAnimationIdxMax;
  }
}

void Character::runAnimation(State state, const function<void ()>& func) const {
  auto animate = Animate::create(_bodyAnimations[state]);
  auto callback = CallFunc::create(func);
  _bodySprite->stopAllActions();
  _bodySprite->runAction(Sequence::createWithTwoActions(animate, callback));

  // Update equipment animation.
  for (int type = 0; type < static_cast<int>(Equipment::Type::SIZE); type++) {
    if (!_equipmentSlots[type]) {
      continue;
    }

    Animate* animate = Animate::create(_equipmentAnimations[type][state]);
    Action* action = Repeat::create(animate, 1);
    _equipmentSprites[type]->stopAllActions();
    _equipmentSprites[type]->runAction(action);
  }
}

void Character::runAnimation(const string& framesName, float interval) {
  // Try to load the target framesName under this character's textureResDir.
  Animation* bodyAnimation = nullptr;
  
  if (_skillBodyAnimations.find(framesName) != _skillBodyAnimations.end()) {
    bodyAnimation = _skillBodyAnimations[framesName];
  } else {
    Animation* fallback = _bodyAnimations[State::ATTACKING];
    bodyAnimation = createAnimation(_characterProfile.textureResDir, framesName, interval, fallback);
    // Cache this skill animation (body).
    _skillBodyAnimations.insert({framesName, bodyAnimation});
  }

  _bodySprite->stopAllActions();
  _bodySprite->runAction(Repeat::create(Animate::create(bodyAnimation), 1));

  // Update equipment animation.
  for (int type = 0; type < static_cast<int>(Equipment::Type::SIZE); type++) {
    if (!_equipmentSlots[type]) {
      continue;
    }

    const string& textureResDir = _equipmentSlots[type]->getItemProfile().textureResDir;
    Animation* fallback = _equipmentAnimations[type][ATTACKING];

    // TODO: cache these equipment skill animation
    Animation* animation = createAnimation(textureResDir, framesName, interval, fallback);
    _equipmentSprites[type]->stopAllActions();
    _equipmentSprites[type]->runAction(Animate::create(animation));
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
  } else if (_body->GetLinearVelocity().y < -1.0f && !_isTakingDamage) {
    return (_isWeaponSheathed) ? State::FALLING_SHEATHED : State::FALLING_UNSHEATHED;
  } else if (_isCrouching) {
    return (_isWeaponSheathed) ? State::CROUCHING_SHEATHED : State::CROUCHING_UNSHEATHED;
  } else if (std::abs(_body->GetLinearVelocity().x) > .01f && !_isTakingDamage) {
    return (_isWeaponSheathed) ? State::RUNNING_SHEATHED : State::RUNNING_UNSHEATHED;
  } else {
    return (_isWeaponSheathed) ? State::IDLE_SHEATHED : State::IDLE_UNSHEATHED;
  }
}


void Character::onKilled() {
  _isKilled = true;
  GameMapManager::getInstance()->getWorld()->DestroyBody(_body);
}


void Character::moveLeft() {
  _isFacingRight = false;
  if (_body->GetLinearVelocity().x >= -_characterProfile.moveSpeed * 2) {
    _body->ApplyLinearImpulse({-_characterProfile.moveSpeed, 0}, _body->GetWorldCenter(), true);
  }
}

void Character::moveRight() {
  _isFacingRight = true;
  if (_body->GetLinearVelocity().x <= _characterProfile.moveSpeed * 2) {
    _body->ApplyLinearImpulse({_characterProfile.moveSpeed, 0}, _body->GetWorldCenter(), true);
  }
}

void Character::jump() {
  // Block current jump request if:
  // 1. This character cannot double jump, and it has already jumped.
  // 2. This character can double jump, and it has already double jumped.
  const bool& canDoubleJump = _characterProfile.canDoubleJump;
  if ((!canDoubleJump && _isJumping) || (canDoubleJump && _isDoubleJumping)) {
    return;
  }

  if (_isJumping) {
    _isDoubleJumping = true;
    runAnimation((_isWeaponSheathed) ? State::JUMPING_SHEATHED : State::JUMPING_UNSHEATHED, false);
    // Set velocity.y to 0.
    b2Vec2 velocity = _body->GetLinearVelocity();
    _body->SetLinearVelocity({velocity.x, 0});
  } 

  _isJumping = true;
  _body->ApplyLinearImpulse({0, _characterProfile.jumpHeight}, _body->GetWorldCenter(), true);
}

void Character::doubleJump() {
  jump();

  CallbackManager::getInstance()->runAfter([this]() {
    jump();
  }, .25f);
}

void Character::jumpDown() {
  if (!_isOnPlatform) {
    return;
  }

  _fixtures[FixtureType::FEET]->SetSensor(true);

  CallbackManager::getInstance()->runAfter([this]() {
    _fixtures[FixtureType::FEET]->SetSensor(false);
  }, .25f);
}

void Character::crouch() {
  _isCrouching = true;
}

void Character::getUp() {
  _isCrouching = false;
}


void Character::sheathWeapon() {
  _isSheathingWeapon = true;

  CallbackManager::getInstance()->runAfter([this]() {
    _isSheathingWeapon = false;
    _isWeaponSheathed = true;
  }, .8f);
}

void Character::unsheathWeapon() {
  _isUnsheathingWeapon = true;

  CallbackManager::getInstance()->runAfter([this]() {
    _isUnsheathingWeapon = false;
    _isWeaponSheathed = false;
  }, .8f);
}

void Character::attack() {
  // If character is still attacking, block this attack request.
  // The latter condition prevents the character from being stucked in an
  // attack animation when the user calls Character::attack() too frequently.
  if (_isAttacking || _currentState == State::ATTACKING) {
    return;
  }
  if (_isWeaponSheathed) {
    unsheathWeapon();
    return;
  }

  _isAttacking = true;

  CallbackManager::getInstance()->runAfter([this]() {
    _isAttacking = false;
  }, _characterProfile.attackTime);


  if (!_inRangeTargets.empty()) {
    _lockedOnTarget = *_inRangeTargets.begin();

    if (!_lockedOnTarget->isInvincible()) {
      // If this character is not the Player,
      // then add a little delay before inflicting damage / knockback.
      float damageDelay = (dynamic_cast<Player*>(this)) ? 0 : .25f;

      CallbackManager::getInstance()->runAfter([this]() {
          inflictDamage(_lockedOnTarget, getDamageOutput());
          float knockBackForceX = (_isFacingRight) ? .5f : -.5f; // temporary
          float knockBackForceY = 1.0f; // temporary
          knockBack(_lockedOnTarget, knockBackForceX, knockBackForceY);
      }, damageDelay);
    }
  }
}

void Character::activateSkill(Skill* skill) {
  // If this character is still using another skill, or
  // if it doesn't meet the criteria of activating this skill,
  // then return at once.
  if (_isUsingSkill || !skill->canActivate()) {
    return;
  }

  _isUsingSkill = true;
  _currentlyUsedSkill = skill;

  CallbackManager::getInstance()->runAfter([this]() {
    _isUsingSkill = false;
    // Set _currentState to FORCE_UPDATE so that next time in
    // Character::update the animation is guaranteed to be updated.
    _currentState = State::FORCE_UPDATE;
  }, skill->getSkillProfile().framesDuration);

  if (skill->getSkillProfile().characterFramesName != "") {
    Skill::Profile& skillProfile = skill->getSkillProfile();
    runAnimation(skillProfile.characterFramesName, skillProfile.frameInterval / kPpm);
  }

  // Create an extra copy of this skill object and activate it.
  unique_ptr<Skill> copiedSkill(Skill::create(skill->getSkillProfile().jsonFileName, this));
  Skill* copiedSkillRaw = copiedSkill.get();
  _activeSkills.insert(std::move(copiedSkill));
  copiedSkillRaw->activate();
  
  Hud::getInstance()->updateStatusBars();
}

void Character::knockBack(Character* target, float forceX, float forceY) const {
  b2Body* b2body = target->getBody();
  b2body->ApplyLinearImpulse({forceX, forceY}, b2body->GetWorldCenter(), true);
}

void Character::inflictDamage(Character* target, int damage) {
  target->receiveDamage(this, damage);
  target->lockOn(this);

  for (const auto& ally : this->getAllies()) {
    ally->lockOn(target);
  }
  for (const auto& targetAlly : target->getAllies()) {
    targetAlly->lockOn(this);
  }
}

void Character::receiveDamage(Character* source, int damage) {
  if (_isInvincible) {
    return;
  }

  _characterProfile.health -= damage;

  _isTakingDamage = true;
  CallbackManager::getInstance()->runAfter([this]() {
    _isTakingDamage = false;
  }, .25f);
  
  if (_characterProfile.health <= 0) {
    _characterProfile.health = 0;

    source->getInRangeTargets().erase(this);
    for (const auto& sourceAlly : source->getAllies()) {
      sourceAlly->getInRangeTargets().erase(this);
      if (sourceAlly->getLockedOnTarget() == this) {
        sourceAlly->setLockedOnTarget(nullptr);
      }
    }

    DynamicActor::setCategoryBits(_fixtures[FixtureType::BODY], category_bits::kDestroyed);
    _isSetToKill = true;
    // TODO: play killed sound.
  } else {
    // TODO: play hurt sound.
  }

  FloatingDamages::getInstance()->show(this, damage);
}

void Character::lockOn(Character* target) {
  _isAlerted = true;
  setLockedOnTarget(target);
}


void Character::addItem(shared_ptr<Item> item, int amount) {
  if (!item || amount == 0) {
    VGLOG(LOG_WARN, "Either item == nullptr or amount == 0");
    return;
  }

  // If this Item* does not exist in Inventory or EquipmentSlots yet, store it in _itemMapper.
  // Otherwise, simply delete it and use the existing copy instead (saves memory).
  Item* existingItemObj = getExistingItemObj(item.get());

  if (existingItemObj) {
    existingItemObj->setAmount(existingItemObj->getAmount() + amount);
  } else {
    existingItemObj = item.get();
    existingItemObj->setAmount(amount);
    _itemMapper[item->getItemProfile().name] = std::move(item);
  }

  _inventory[existingItemObj->getItemProfile().itemType].insert(existingItemObj);
}

void Character::removeItem(Item* item, int amount) {
  Item* existingItemObj = getExistingItemObj(item);
  
  if (!existingItemObj || amount == 0) {
    VGLOG(LOG_WARN, "Unable to remove such item!");
    return;
  }

  existingItemObj->setAmount(existingItemObj->getAmount() - amount);

  if (existingItemObj->getAmount() <= 0) {
    _inventory[item->getItemProfile().itemType].erase(existingItemObj);

    // We can safely delete this Item* if:
    // 1. It is not an equipment, or...
    // 2. It is an equipment, but no same item is currently equipped.
    Equipment* equipment = dynamic_cast<Equipment*>(existingItemObj);

    if (!equipment ||
        _equipmentSlots[equipment->getEquipmentProfile().equipmentType] != existingItemObj) {
      _itemMapper.erase(item->getItemProfile().name);
    }
  }
}

// For each instance of an item, at most one copy is kept in the memory.
// This copy will be stored in _itemMapper (unordered_map<string, Item*>)
// Search time complexity: avg O(1), worst O(n).
Item* Character::getExistingItemObj(Item* item) const {
  if (!item) {
    return nullptr;
  }
  auto it = _itemMapper.find(item->getItemProfile().name);
  return (it != _itemMapper.end()) ? it->second.get() : nullptr;
}

void Character::useItem(Consumable* consumable) {
  auto& profile = _characterProfile;
  const auto& consumableProfile = consumable->getConsumableProfile();

  profile.health += consumableProfile.restoreHealth;
  if (profile.health > profile.fullHealth) profile.health = profile.fullHealth;

  profile.magicka += consumableProfile.restoreMagicka;
  if (profile.magicka > profile.fullMagicka) profile.magicka = profile.fullMagicka;

  profile.stamina += consumableProfile.restoreStamina;
  if (profile.stamina > profile.fullStamina) profile.stamina = profile.fullStamina;

  profile.baseMeleeDamage += consumableProfile.bonusPhysicalDamage;
  //profile.baseMagicalDamage += consumableProfile.bonusMagicalDamage;
  profile.strength += consumableProfile.bonusStr;
  profile.dexterity += consumableProfile.bonusDex;
  profile.intelligence += consumableProfile.bonusInt;
  profile.luck += consumableProfile.bonusLuk;

  profile.moveSpeed += consumableProfile.bonusMoveSpeed;
  profile.jumpHeight += consumableProfile.bonusJumpHeight;

  Hud::getInstance()->updateStatusBars();
  removeItem(consumable, 1);
}

void Character::equip(Equipment* equipment) {
  // If there's already an equipment in that slot, unequip it first.
  Equipment::Type type = equipment->getEquipmentProfile().equipmentType;
  if (_equipmentSlots[type]) {
    unequip(type);
  }
  _equipmentSlots[type] = equipment;
  removeItem(equipment, 1);

  // Load equipment animations.
  loadEquipmentAnimations(equipment);
  GameMapManager::getInstance()->getLayer()->addChild(_equipmentSpritesheets[type], graphical_layers::kEquipment - type);
}

void Character::unequip(Equipment::Type equipmentType) {
  // If there's an equipped item in the target slot,
  // move it into character's inventory.
  if (!_equipmentSlots[equipmentType]) {
    return;
  }

  if (equipmentType == Equipment::Type::WEAPON) {
    sheathWeapon();
  }

  Equipment* e = _equipmentSlots[equipmentType];
  _equipmentSlots[equipmentType] = nullptr;
  addItem(_itemMapper.find(e->getItemProfile().name)->second, 1);
  GameMapManager::getInstance()->getLayer()->removeChild(_equipmentSpritesheets[equipmentType]);
}

void Character::pickupItem(Item* item) {
  shared_ptr<Item> i = GameMapManager::getInstance()->getGameMap()->removeDynamicActor<Item>(item); 
  addItem(std::move(i), item->getAmount());
}

void Character::discardItem(Item* item, int amount) {
  const string& jsonFileName = item->getItemProfile().jsonFileName;
  float x = _body->GetPosition().x;
  float y = _body->GetPosition().y;
  GameMapManager::getInstance()->getGameMap()->createItem(jsonFileName, x * kPpm, y * kPpm, amount);

  removeItem(item, amount);
}

void Character::interact(Interactable* target) {
  target->onInteract(this);
}

void Character::addExp(const int exp) {
  int& thisExp = _characterProfile.exp;
  int& thisLevel = _characterProfile.level;

  thisExp += exp;
  while (thisExp >= exp_point_table::getNextLevelExp(thisLevel)) {
    thisExp -= exp_point_table::getNextLevelExp(thisLevel);
    thisLevel++;
  }
}


void Character::addSkill(unique_ptr<Skill> skill) {
  assert(skill != nullptr);

  auto it = _skillMapper.find(skill->getName());
  if (it != _skillMapper.end()) {
    VGLOG(LOG_WARN, "This character has already learned the skill: %s", skill->getName().c_str());
    return;
  }

  _skillBook[skill->getSkillProfile().skillType].insert(skill.get());
  _skillMapper.insert({skill->getName(), std::move(skill)});
}

void Character::removeSkill(Skill* skill) {
  assert(skill != nullptr);

  auto it = _skillMapper.find(skill->getName());
  if (it == _skillMapper.end()) {
    VGLOG(LOG_WARN, "This character has not yet learned the skill: %s", skill->getName().c_str());
    return;
  }

  _skillBook[skill->getSkillProfile().skillType].erase(skill);
  _skillMapper.erase(skill->getName());
}


int Character::getGoldBalance() const {
  return getItemAmount(Item::create(asset_manager::kGoldCoin)->getName());
}

void Character::addGold(const int amount) {
  addItem(Item::create(asset_manager::kGoldCoin), amount);
}

void Character::removeGold(const int amount) {
  removeItem(Item::create(asset_manager::kGoldCoin).get(), amount);
}


bool Character::isFacingRight() const {
  return _isFacingRight;
}

bool Character::isJumping() const {
  return _isJumping;
}

bool Character::isDoubleJumping() const {
  return _isDoubleJumping;
}

bool Character::isOnPlatform() const {
  return _isOnPlatform;
}

bool Character::isAttacking() const {
  return _isAttacking;
}

bool Character::isUsingSkill() const {
  return _isUsingSkill;
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

void Character::setDoubleJumping(bool doubleJumping) {
  _isDoubleJumping = doubleJumping;
}

void Character::setOnPlatform(bool onPlatform) {
  _isOnPlatform = onPlatform;
}

void Character::setAttacking(bool attacking) {
  _isAttacking = attacking;
}

void Character::setUsingSkill(bool usingSkill) {
  _isUsingSkill = usingSkill;
}

void Character::setCrouching(bool crouching) {
  _isCrouching = crouching;
}

void Character::setInvincible(bool invincible) {
  _isInvincible = invincible;
}


Character::Profile& Character::getCharacterProfile() {
  return _characterProfile;
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


int Character::getItemAmount(const string& itemName) const {
  if (_itemMapper.find(itemName) == _itemMapper.end()) {
    return 0;
  }
  return _itemMapper.at(itemName)->getAmount();
}


Interactable* Character::getInteractableObject() const {
  return _interactableObject;
}

void Character::setInteractableObject(Interactable* interactableObject) {
  _interactableObject = interactableObject;
}

GameMap::Portal* Character::getPortal() const {
  return _portal;
}

void Character::setPortal(GameMap::Portal* portal) {
  _portal = portal;
}


const Character::SkillBook& Character::getSkillBook() const {
  return _skillBook;
}

unordered_set<shared_ptr<Skill>>& Character::getActiveSkills() {
  // FIXME: make add/remove/get consistent with allies
  return _activeSkills;
}

Skill* Character::getCurrentlyUsedSkill() const {
  return _currentlyUsedSkill;
}


bool Character::isWaitingForPartyLeader() const {
  return _party && _party->hasWaitingMember(_characterProfile.jsonFileName);
}

unordered_set<Character*> Character::getAllies() const {
  if (!_party) {
    return {};
  }

  unordered_set<Character*> ret;

  for (const auto& member : _party->getMembers()) {
    ret.insert(member.get());
  }

  Character* leader = _party->getLeader();
  if (leader != this) {
    ret.insert(leader);
  }

  return ret;
}

shared_ptr<Party> Character::getParty() const {
  return _party;
}

void Character::setParty(shared_ptr<Party> party) {
  _party = party;
}



int Character::getDamageOutput() const {
  int output = _characterProfile.baseMeleeDamage;

  Equipment* weapon = _equipmentSlots[Equipment::Type::WEAPON];
  if (weapon) {
    output += weapon->getEquipmentProfile().bonusPhysicalDamage;
  }

  return output + rand_util::randInt(-5, 5); // temporary
}


void Character::regenHealth(int deltaHealth) {
  const int& fullHealth = _characterProfile.fullHealth;
  int& health = _characterProfile.health;

  health += deltaHealth;
  health = (health > fullHealth) ? fullHealth : health;
}

void Character::regenMagicka(int deltaMagicka) {
  const int& fullMagicka = _characterProfile.fullMagicka;
  int& magicka = _characterProfile.magicka;

  magicka += deltaMagicka;
  magicka = (magicka > fullMagicka) ? fullMagicka : magicka;
}

void Character::regenStamina(int deltaStamina) {
  const int& fullStamina = _characterProfile.fullStamina;
  int& stamina = _characterProfile.stamina;

  stamina += deltaStamina;
  stamina = (stamina > fullStamina) ? fullStamina : stamina;
}



Character::Profile::Profile(const string& jsonFileName) : jsonFileName(jsonFileName) {
  Document json = json_util::parseJson(jsonFileName);

  textureResDir = json["textureResDir"].GetString();
  spriteOffsetX = json["spriteOffsetX"].GetFloat();
  spriteOffsetY = json["spriteOffsetY"].GetFloat();
  spriteScaleX = json["spriteScaleX"].GetFloat();
  spriteScaleY = json["spriteScaleY"].GetFloat();

  for (int i = 0; i < Character::State::STATE_SIZE; i++) {
    float interval = json["frameInterval"][Character::_kCharacterStateStr[i].c_str()].GetFloat();
    frameInterval.push_back(interval);
  }

  name = json["name"].GetString();
  level = json["level"].GetInt();
  exp = json["exp"].GetInt();

  fullHealth = json["fullHealth"].GetInt();
  fullStamina = json["fullStamina"].GetInt();
  fullMagicka = json["fullMagicka"].GetInt();

  health = json["health"].GetInt();
  stamina = json["stamina"].GetInt();
  magicka = json["magicka"].GetInt();

  strength = json["strength"].GetInt();
  dexterity = json["dexterity"].GetInt();
  intelligence = json["intelligence"].GetInt();
  luck = json["luck"].GetInt();

  bodyWidth = json["bodyWidth"].GetInt();
  bodyHeight = json["bodyHeight"].GetInt();
  moveSpeed = json["moveSpeed"].GetFloat();
  jumpHeight = json["jumpHeight"].GetFloat();
  canDoubleJump = json["canDoubleJump"].GetBool();

  attackForce = json["attackForce"].GetFloat();
  attackTime = json["attackTime"].GetFloat();
  attackRange = json["attackRange"].GetFloat();
  baseMeleeDamage = json["baseMeleeDamage"].GetInt();

  for (const auto& skillJson : json["defaultSkills"].GetArray()) {
    string skillJsonFileName = skillJson.GetString();
    defaultSkills.push_back(skillJsonFileName);
  }

  for (const auto& itemJson : json["defaultInventory"].GetObject()) {
    string itemJsonFileName = itemJson.name.GetString();
    int amount = itemJson.value.GetInt();
    defaultInventory.push_back({itemJsonFileName, amount});
  }
}

}  // namespace vigilante
