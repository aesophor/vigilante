#include "Character.h"

#include "json/document.h"

#include "AssetManager.h"
#include "Constants.h"
#include "CategoryBits.h"
#include "GraphicalLayers.h"
#include "map/GameMapManager.h"
#include "ui/hud/Hud.h"
#include "ui/damage/FloatingDamageManager.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/CallbackUtil.h"
#include "util/RandUtil.h"
#include "util/JsonUtil.h"

using std::set;
using std::array;
using std::vector;
using std::unordered_map;
using std::string;
using std::function;
using std::ifstream;
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
  "attacking",
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
      _isKilled(),
      _isSetToKill(),
      _inRangeTargets(),
      _lockedOnTarget(),
      _isAlerted(),
      _inventory(),
      _equipmentSlots(),
      _interactableObject(),
      _portal(),
      _skills(),
      _currentlyUsedSkill(),
      _bodyExtraAttackAnimations(),
      _equipmentExtraAttackAnimations(),
      _equipmentSprites(),
      _equipmentSpritesheets(),
      _equipmentAnimations(),
      _skillBodyAnimations() {}

Character::~Character() {
  // Delete all items from inventory and equipment slots.
  for (auto item : _itemMapper) {
    delete item.second;
  }
  // Delete all skills.
  for (auto skill : _skills) {
    delete skill;
  }
}


void Character::update(float delta) {
  if (_isKilled) return;

  // Flip the sprite if needed.
  if (!_isFacingRight && !_bodySprite->isFlippedX()) {
    _bodySprite->setFlippedX(true);
    b2CircleShape* shape = static_cast<b2CircleShape*>(_fixtures[FixtureType::WEAPON]->GetShape());
    shape->m_p = {-_characterProfile.attackRange / kPpm, 0};
  } else if (_isFacingRight && _bodySprite->isFlippedX()) {
    _bodySprite->setFlippedX(false);
    b2CircleShape* shape = static_cast<b2CircleShape*>(_fixtures[FixtureType::WEAPON]->GetShape());
    shape->m_p = {_characterProfile.attackRange / kPpm, 0};
  }

  // Sync the body sprite with its b2body.
  b2Vec2 b2bodyPos = _body->GetPosition();
  _bodySprite->setPosition(b2bodyPos.x * kPpm, b2bodyPos.y * kPpm + _characterProfile.spriteOffsetY);

  // Sync the equipment sprites with its b2body.
  for (int i = 0; i < Equipment::Type::SIZE; i++) {
    Equipment::Type type = static_cast<Equipment::Type>(i);
    if (_equipmentSlots[type]) {
      if (!_isFacingRight && !_equipmentSprites[type]->isFlippedX()) {
        _equipmentSprites[type]->setFlippedX(true);
      } else if (_isFacingRight && _equipmentSprites[type]->isFlippedX()) {
        _equipmentSprites[type]->setFlippedX(false);
      }
      _equipmentSprites[type]->setPosition(b2bodyPos.x * kPpm, b2bodyPos.y * kPpm + _characterProfile.spriteOffsetY);
    }
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
}

void Character::setPosition(float x, float y) {
  _body->SetTransform({x, y}, 0);
}

void Character::removeFromMap() {
  if (_isShownOnMap) {
    if (!_isKilled) {
      _body->GetWorld()->DestroyBody(_body);
    }

    GameMapManager* gmMgr = GameMapManager::getInstance();
    gmMgr->getLayer()->removeChild(_bodySpritesheet);
    for (auto equipment : _equipmentSlots) {
      if (equipment) {
        Equipment::Type type = equipment->getEquipmentProfile().equipmentType;
        gmMgr->getLayer()->removeChild(_equipmentSpritesheets[type]);
      }
    }

    _isShownOnMap = false;
  }
}

void Character::import(const string& jsonFileName) {
  _characterProfile = Character::Profile(jsonFileName);
}


void Character::defineBody(b2BodyType bodyType, short bodyCategoryBits, short bodyMaskBits,
                           short feetMaskBits, short weaponMaskBits, float x, float y) {
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
  _bodySpritesheet = SpriteBatchNode::create(bodyTextureResDir + "/spritesheet.png");

  _bodyAnimations[State::IDLE_SHEATHED] = createAnimation(bodyTextureResDir, _kCharacterStateStr[State::IDLE_SHEATHED], _characterProfile.frameInterval[State::IDLE_SHEATHED] / kPpm);
  Animation* fallback = _bodyAnimations[State::IDLE_SHEATHED];
  _bodyAnimations[State::IDLE_UNSHEATHED] = createAnimation(bodyTextureResDir, _kCharacterStateStr[State::IDLE_UNSHEATHED], _characterProfile.frameInterval[State::IDLE_UNSHEATHED] / kPpm, fallback);
  _bodyAnimations[State::RUNNING_SHEATHED] = createAnimation(bodyTextureResDir, _kCharacterStateStr[State::RUNNING_SHEATHED], _characterProfile.frameInterval[State::RUNNING_SHEATHED] / kPpm, fallback);
  _bodyAnimations[State::RUNNING_UNSHEATHED] = createAnimation(bodyTextureResDir, _kCharacterStateStr[State::RUNNING_UNSHEATHED], _characterProfile.frameInterval[State::RUNNING_UNSHEATHED] / kPpm, fallback);
  _bodyAnimations[State::JUMPING_SHEATHED] = createAnimation(bodyTextureResDir, _kCharacterStateStr[State::JUMPING_SHEATHED], _characterProfile.frameInterval[State::JUMPING_SHEATHED] / kPpm, fallback);
  _bodyAnimations[State::JUMPING_UNSHEATHED] = createAnimation(bodyTextureResDir, _kCharacterStateStr[State::JUMPING_UNSHEATHED], _characterProfile.frameInterval[State::JUMPING_UNSHEATHED] / kPpm, fallback);
  _bodyAnimations[State::FALLING_SHEATHED] = createAnimation(bodyTextureResDir, _kCharacterStateStr[State::FALLING_SHEATHED], _characterProfile.frameInterval[State::FALLING_SHEATHED] / kPpm, fallback);
  _bodyAnimations[State::FALLING_UNSHEATHED] = createAnimation(bodyTextureResDir, _kCharacterStateStr[State::FALLING_UNSHEATHED], _characterProfile.frameInterval[State::FALLING_UNSHEATHED] / kPpm, fallback);
  _bodyAnimations[State::CROUCHING_SHEATHED] = createAnimation(bodyTextureResDir, _kCharacterStateStr[State::CROUCHING_SHEATHED], _characterProfile.frameInterval[State::CROUCHING_SHEATHED] / kPpm, fallback);
  _bodyAnimations[State::CROUCHING_UNSHEATHED] = createAnimation(bodyTextureResDir, _kCharacterStateStr[State::CROUCHING_UNSHEATHED], _characterProfile.frameInterval[State::CROUCHING_UNSHEATHED] / kPpm, fallback);
  _bodyAnimations[State::SHEATHING_WEAPON] = createAnimation(bodyTextureResDir, _kCharacterStateStr[State::SHEATHING_WEAPON], _characterProfile.frameInterval[State::SHEATHING_WEAPON] / kPpm, fallback);
  _bodyAnimations[State::UNSHEATHING_WEAPON] = createAnimation(bodyTextureResDir, _kCharacterStateStr[State::UNSHEATHING_WEAPON], _characterProfile.frameInterval[State::UNSHEATHING_WEAPON] / kPpm, fallback);
  _bodyAnimations[State::ATTACKING] = createAnimation(bodyTextureResDir, _kCharacterStateStr[State::ATTACKING], _characterProfile.frameInterval[State::ATTACKING] / kPpm, fallback);
  _bodyAnimations[State::KILLED] = createAnimation(bodyTextureResDir, _kCharacterStateStr[State::KILLED], _characterProfile.frameInterval[State::KILLED] / kPpm, fallback);

  // Load extra attack animations.
  _bodyExtraAttackAnimations[0] = createAnimation(bodyTextureResDir, "attacking2", _characterProfile.frameInterval[State::ATTACKING] / kPpm, fallback);

  // Select a frame as default look for this sprite.
  string framePrefix = StaticActor::getLastDirName(bodyTextureResDir);
  _bodySprite = Sprite::createWithSpriteFrameName(framePrefix + "_idle_sheathed/0.png");
  _bodySprite->setScaleX(_characterProfile.spriteScaleX);
  _bodySprite->setScaleY(_characterProfile.spriteScaleY);

  _bodySpritesheet->addChild(_bodySprite);
  _bodySpritesheet->getTexture()->setAliasTexParameters(); // disable texture antialiasing
}

void Character::loadEquipmentAnimations(Equipment* equipment) {
  Equipment::Type type = equipment->getEquipmentProfile().equipmentType;
  const string& textureResDir = equipment->getItemProfile().textureResDir;
  _equipmentSpritesheets[type] = SpriteBatchNode::create(textureResDir + "/spritesheet.png");

  _equipmentAnimations[type][State::IDLE_SHEATHED] = createAnimation(textureResDir, _kCharacterStateStr[State::IDLE_SHEATHED], _characterProfile.frameInterval[State::IDLE_SHEATHED] / kPpm);
  Animation* fallback = _equipmentAnimations[type][State::IDLE_SHEATHED];
  _equipmentAnimations[type][State::IDLE_UNSHEATHED] = createAnimation(textureResDir, _kCharacterStateStr[State::IDLE_UNSHEATHED], _characterProfile.frameInterval[State::IDLE_UNSHEATHED] / kPpm, fallback);
  _equipmentAnimations[type][State::RUNNING_SHEATHED] = createAnimation(textureResDir, _kCharacterStateStr[State::RUNNING_SHEATHED], _characterProfile.frameInterval[State::RUNNING_SHEATHED] / kPpm, fallback);
  _equipmentAnimations[type][State::RUNNING_UNSHEATHED] = createAnimation(textureResDir, _kCharacterStateStr[State::RUNNING_UNSHEATHED], _characterProfile.frameInterval[State::RUNNING_UNSHEATHED] / kPpm, fallback);
  _equipmentAnimations[type][State::JUMPING_SHEATHED] = createAnimation(textureResDir, _kCharacterStateStr[State::JUMPING_SHEATHED], _characterProfile.frameInterval[State::JUMPING_SHEATHED] / kPpm, fallback);
  _equipmentAnimations[type][State::JUMPING_UNSHEATHED] = createAnimation(textureResDir, _kCharacterStateStr[State::JUMPING_UNSHEATHED], _characterProfile.frameInterval[State::JUMPING_UNSHEATHED] / kPpm, fallback);
  _equipmentAnimations[type][State::FALLING_SHEATHED] = createAnimation(textureResDir, _kCharacterStateStr[State::FALLING_SHEATHED], _characterProfile.frameInterval[State::FALLING_SHEATHED] / kPpm, fallback);
  _equipmentAnimations[type][State::FALLING_UNSHEATHED] = createAnimation(textureResDir, _kCharacterStateStr[State::FALLING_UNSHEATHED], _characterProfile.frameInterval[State::FALLING_UNSHEATHED] / kPpm, fallback);
  _equipmentAnimations[type][State::CROUCHING_SHEATHED] = createAnimation(textureResDir, _kCharacterStateStr[State::CROUCHING_SHEATHED], _characterProfile.frameInterval[State::CROUCHING_SHEATHED] / kPpm, fallback);
  _equipmentAnimations[type][State::CROUCHING_UNSHEATHED] = createAnimation(textureResDir, _kCharacterStateStr[State::CROUCHING_UNSHEATHED], _characterProfile.frameInterval[State::CROUCHING_UNSHEATHED] / kPpm, fallback);
  _equipmentAnimations[type][State::SHEATHING_WEAPON] = createAnimation(textureResDir, _kCharacterStateStr[State::SHEATHING_WEAPON], _characterProfile.frameInterval[State::SHEATHING_WEAPON] / kPpm, fallback);
  _equipmentAnimations[type][State::UNSHEATHING_WEAPON] = createAnimation(textureResDir, _kCharacterStateStr[State::UNSHEATHING_WEAPON], _characterProfile.frameInterval[State::UNSHEATHING_WEAPON] / kPpm, fallback);
  _equipmentAnimations[type][State::ATTACKING] = createAnimation(textureResDir, _kCharacterStateStr[State::ATTACKING], _characterProfile.frameInterval[State::ATTACKING] / kPpm, fallback);
  _equipmentAnimations[type][State::KILLED] = createAnimation(textureResDir, _kCharacterStateStr[State::KILLED], _characterProfile.frameInterval[State::KILLED] / kPpm, fallback);

  // Load extra attack animations.
  _equipmentExtraAttackAnimations[type][0] = createAnimation(textureResDir, "attacking2", _characterProfile.frameInterval[State::ATTACKING] / kPpm, fallback);

  string framePrefix = StaticActor::getLastDirName(textureResDir);
  _equipmentSprites[type] = Sprite::createWithSpriteFrameName(framePrefix + "_idle_sheathed/0.png");
  _equipmentSprites[type]->setScaleX(_characterProfile.spriteScaleX);
  _equipmentSprites[type]->setScaleY(_characterProfile.spriteScaleY);

  _equipmentSpritesheets[type]->addChild(_equipmentSprites[type]);
  _equipmentSpritesheets[type]->getTexture()->setAliasTexParameters();
}


void Character::runAnimation(State state, bool loop) const {
  auto targetAnimation = _bodyAnimations[state];

  int attackAnimationIdx = 0;
  if (state == State::ATTACKING) {
    int i = rand_util::randInt(0, 1);
    if (i >= 1) {
      // Pick the animation from _extraAttackAnimations array.
      targetAnimation = _bodyExtraAttackAnimations[i - 1];
    }
    attackAnimationIdx = i;
  }

  auto animation = Animate::create(targetAnimation);
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

      auto targetAnimation = _equipmentAnimations[type][state];
      if (state == State::ATTACKING && attackAnimationIdx > 0) {
        targetAnimation = _equipmentExtraAttackAnimations[type][attackAnimationIdx - 1];
      }

      auto animate = Animate::create(targetAnimation);
      Action* action = nullptr;
      if (loop) {
        action = RepeatForever::create(animate);
      } else {
        action = Repeat::create(animate, 1);
      }
      _equipmentSprites[type]->runAction(action);
    }
  }
}

void Character::runAnimation(State state, const function<void ()>& func) const {
  auto animate = Animate::create(_bodyAnimations[state]);
  auto callback = CallFunc::create(func);
  _bodySprite->stopAllActions();
  _bodySprite->runAction(Sequence::createWithTwoActions(animate, callback));

  // Update equipment animation.
  for (int i = 0; i < Equipment::Type::SIZE; i++) {
    Equipment::Type type = static_cast<Equipment::Type>(i);
    if (_equipmentSlots[type]) {
      _equipmentSprites[type]->stopAllActions();

      auto animate = Animate::create(_equipmentAnimations[type][state]);
      Action* action = Repeat::create(animate, 1);
      _equipmentSprites[type]->runAction(action);
    }
  }
}

void Character::runAnimation(const string& framesName, float interval) {
  // Try to load the target framesName under this character's textureResDir.
  Animation* bodyAnimation = nullptr;
  
  if (_skillBodyAnimations.find(framesName) == _skillBodyAnimations.end()) {
    Animation* fallback = _bodyAnimations[State::ATTACKING];
    bodyAnimation = createAnimation(_characterProfile.textureResDir, framesName, interval, fallback);
    // Cache this skill animation (body).
    _skillBodyAnimations.insert({framesName, bodyAnimation});
  } else {
    bodyAnimation = _skillBodyAnimations[framesName];
  }

  _bodySprite->stopAllActions();
  _bodySprite->runAction(Repeat::create(Animate::create(bodyAnimation), 1));

  // Update equipment animation.
  for (int i = 0; i < Equipment::Type::SIZE; i++) {
    Equipment::Type type = static_cast<Equipment::Type>(i);
    if (_equipmentSlots[type]) {
      _equipmentSprites[type]->stopAllActions();

      const string& textureResDir = _equipmentSlots[type]->getItemProfile().textureResDir;
      Animation* fallback = _equipmentAnimations[type][ATTACKING];

      // TODO: cache these equipment skill animation
      Animation* animation = createAnimation(textureResDir, framesName, interval, fallback);
      _equipmentSprites[type]->stopAllActions();
      _equipmentSprites[type]->runAction(Animate::create(animation));
    }
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
  } else if (_body->GetLinearVelocity().y < -1.0f) {
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

  if (!_isJumping) {
    _isJumping = true;
  } else {
    _isDoubleJumping = true;
    runAnimation((_isWeaponSheathed) ? State::JUMPING_SHEATHED : State::JUMPING_UNSHEATHED, false);
    // Set velocity.y to 0.
    b2Vec2 velocity = _body->GetLinearVelocity();
    _body->SetLinearVelocity({velocity.x, 0});
  } 

  _isJumping = true;
  _body->ApplyLinearImpulse({0, _characterProfile.jumpHeight}, _body->GetWorldCenter(), true);
}

void Character::jumpDown() {
  if (_isOnPlatform) {
    _fixtures[FixtureType::FEET]->SetSensor(true);

    callback_util::runAfter([=]() {
      _fixtures[FixtureType::FEET]->SetSensor(false);
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
  // If character is still attacking, block this request.
  // The latter condition prevents the character from being stucked in an
  // attack animation when the user calls Character::attack too frequently.
  if (_isAttacking || _currentState == State::ATTACKING) {
    return;
  }
  if (_isWeaponSheathed) {
    unsheathWeapon();
    return;
  }

  _isAttacking = true;

  callback_util::runAfter([=]() {
    _isAttacking = false;
  }, _characterProfile.attackTime);

  if (!_inRangeTargets.empty()) {
    _lockedOnTarget = *_inRangeTargets.begin();

    if (!_lockedOnTarget->isInvincible()) {
      inflictDamage(_lockedOnTarget, getDamageOutput());
      float knockBackForceX = (isFacingRight()) ? .5f : -.5f; // temporary
      float knockBackForceY = 1.0f; // temporary
      knockBack(_lockedOnTarget, knockBackForceX, knockBackForceY);
    }
  }
}

void Character::activateSkill(Skill* skill) {
  // If this character is still using another skill, or
  // he/she doesn't meet the criteria of activating this skill,
  // delete skill and return at once.
  if (_isUsingSkill || !skill->canActivate(this)) {
    return;
  }

  _isUsingSkill = true;
  _currentlyUsedSkill = skill;

  callback_util::runAfter([=]() {
    _isUsingSkill = false;
    // Set _currentState to FORCE_UPDATE so that next time in
    // Character::update the animation is guaranteed to be updated.
    _currentState = State::FORCE_UPDATE;
  }, skill->getSkillProfile().framesDuration);

  if (skill->getSkillProfile().characterFramesName != "") {
    Skill::Profile& skillProfile = skill->getSkillProfile();
    runAnimation(skillProfile.characterFramesName, skillProfile.frameInterval / kPpm);
  }

  skill->activate(this);
  Hud::getInstance()->updateStatusBars();
}

void Character::knockBack(Character* target, float forceX, float forceY) const {
  b2Body* b2body = target->getBody();
  b2body->ApplyLinearImpulse({forceX, forceY}, b2body->GetWorldCenter(), true);
}

void Character::inflictDamage(Character* target, int damage) {
  target->receiveDamage(this, damage);
  target->lockOn(this);
}

void Character::receiveDamage(Character* source, int damage) {
  if (_isInvincible) {
    return;
  }

  _characterProfile.health -= damage;
  FloatingDamageManager::getInstance()->show(this, damage);

  if (_characterProfile.health <= 0) {
    source->getInRangeTargets().erase(this);
    Character::setCategoryBits(_fixtures[FixtureType::BODY], category_bits::kDestroyed);
    _isSetToKill = true;
    // TODO: play killed sound.
  } else {
    // TODO: play hurt sound.
  }
}

void Character::lockOn(Character* target) {
  _isAlerted = true;
  setLockedOnTarget(target);
}


void Character::addItem(Item* item, int amount) {
  // If this Item* does not exist in Inventory or EquipmentSlots yet, store it in _itemMapper.
  // Otherwise, simply delete it and use the existing copy instead (saves memory).
  Item* existingItemObj = getExistingItemObj(item);

  if (!existingItemObj) {
    existingItemObj = item;
    _itemMapper[item->getItemProfile().name] = item;
  } else {
    existingItemObj->setAmount(existingItemObj->getAmount() + amount);
    if (item != existingItemObj) {
      delete item;
    }
  }

  vector<Item*>& items = _inventory[item->getItemProfile().itemType];
  if (std::find(items.begin(), items.end(), existingItemObj) == items.end()) {
    items.push_back(existingItemObj);
  }
}

void Character::removeItem(Item* item, int amount) {
  vector<Item*>& items = _inventory[item->getItemProfile().itemType];
  Item* existingItemObj = getExistingItemObj(item);
  existingItemObj->setAmount(existingItemObj->getAmount() - amount);

  if (existingItemObj->getAmount() == 0) {
    items.erase(std::remove(items.begin(), items.end(), existingItemObj), items.end());
    // We can safely delete this Item* if:
    // 1. It is not an equipment, or...
    // 2. It is an equipment, but no same item is currently equipped.
    Equipment* equipment = dynamic_cast<Equipment*>(existingItemObj);
    if (!equipment || (_equipmentSlots[equipment->getEquipmentProfile().equipmentType] != existingItemObj)) {
      _itemMapper.erase(item->getItemProfile().name);
      delete existingItemObj;
    }
  }
}

Item* Character::getExistingItemObj(Item* item) const {
  // For each instance of an item, at most one copy is kept in the memory.
  // This copy will be stored in _itemMapper (unordered_map<string, Item*>)
  // Search time complexity: avg O(1), worst O(n).
  auto it = _itemMapper.find(item->getItemProfile().name);
  return (it != _itemMapper.end()) ? (*it).second : nullptr;
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
  if (_equipmentSlots[equipmentType]) {
    Equipment* e = _equipmentSlots[equipmentType];
    _equipmentSlots[equipmentType] = nullptr;
    addItem(e, 1);

    GameMapManager::getInstance()->getLayer()->removeChild(_equipmentSpritesheets[equipmentType]);

    if (equipmentType == Equipment::Type::WEAPON) {
      sheathWeapon();
    }
  }
}

void Character::pickupItem(Item* item) {
  item->removeFromMap();
  GameMapManager::getInstance()->getGameMap()->getDynamicActors().erase(item);

  addItem(item, item->getAmount());
}

void Character::discardItem(Item* item, int amount) {
  const string& jsonFileName = item->getItemProfile().jsonFileName;
  float x = _body->GetPosition().x;
  float y = _body->GetPosition().y;
  GameMapManager::getInstance()->getGameMap()->spawnItem(jsonFileName, x * kPpm, y * kPpm, amount);

  removeItem(item, amount);
}

void Character::interact(Interactable* target) {
  target->onInteract(this);
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


vector<Skill*>& Character::getSkills() {
  return _skills;
}

Skill* Character::getCurrentlyUsedSkill() const {
  return _currentlyUsedSkill;
}


void Character::setCategoryBits(b2Fixture* fixture, short bits) {
  b2Filter filter;
  filter.categoryBits = bits;
  fixture->SetFilterData(filter);
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




Character::Profile::Profile(const string& jsonFileName) {
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
}

} // namespace vigilante
