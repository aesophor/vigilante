// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "MagicalMissile.h"

#include <functional>
#include <memory>

#include "AssetManager.h"
#include "Constants.h"
#include "character/Character.h"
#include "map/GameMapManager.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/Logger.h"

#define MAGICAL_MISSILE_NUM_ANIMATIONS MagicalMissile::AnimationType::SIZE
#define MAGICAL_MISSILE_NUM_FIXTURES 1

#define MAGICAL_MISSILE_CATEOGRY_BITS kProjectile
#define MAGICAL_MISSILE_MASK_BITS kPlayer | kEnemy | kWall

using std::string;
using std::function;
using std::shared_ptr;
using cocos2d::FileUtils;
using cocos2d::Vector;
using cocos2d::Director;
using cocos2d::Action;
using cocos2d::Animate;
using cocos2d::CallFunc;
using cocos2d::Sequence;
using cocos2d::Repeat;
using cocos2d::RepeatForever;
using cocos2d::Sprite;
using cocos2d::SpriteFrame;
using cocos2d::SpriteBatchNode;
using cocos2d::SpriteFrameCache;
using cocos2d::Animation;
using cocos2d::EventKeyboard;
using vigilante::category_bits::kProjectile;
using vigilante::category_bits::kPlayer;
using vigilante::category_bits::kEnemy;
using vigilante::category_bits::kWall;

namespace vigilante {

MagicalMissile::MagicalMissile(const string& jsonFileName, Character* user)
    : DynamicActor(MAGICAL_MISSILE_NUM_ANIMATIONS, MAGICAL_MISSILE_NUM_FIXTURES),
      _skillProfile(jsonFileName),
      _user(user),
      _hasActivated(),
      _hasHit(),
      _launchFxSprite() {}


bool MagicalMissile::showOnMap(float x, float y) {
  if (_isShownOnMap) {
    return false;
  }

  _isShownOnMap = true;

  defineBody(b2BodyType::b2_kinematicBody,
             x,
             y,
             MAGICAL_MISSILE_CATEOGRY_BITS,
             MAGICAL_MISSILE_MASK_BITS);

  defineTexture(_skillProfile.textureResDir, x, y);
  GameMapManager::getInstance()->getLayer()->addChild(_bodySpritesheet,
                                                      graphical_layers::kSpell);
  return true;
}

void MagicalMissile::update(float delta) {
  DynamicActor::update(delta);
  
  // If _body goes out of map, then we can delete this object.
  float x = _body->GetPosition().x * kPpm;
  float y = _body->GetPosition().y * kPpm;
  GameMap* gameMap = GameMapManager::getInstance()->getGameMap();

  if (!_hasHit &&
      (x < 0 || x > gameMap->getWidth() || y < 0 || y > gameMap->getHeight())) {
    onHit(nullptr);
  }
}


int MagicalMissile::getDamage() const {
  return _skillProfile.physicalDamage + _skillProfile.magicalDamage;
}

Character* MagicalMissile::getUser() const {
  return _user;
}

void MagicalMissile::onHit(Character* target) {
  if (_hasHit) {
    return;
  }

  _hasHit = true;
  _body->SetLinearVelocity({_body->GetLinearVelocity().x / 2, 0});

  _bodySprite->runAction(Sequence::createWithTwoActions(
    Animate::create(_bodyAnimations[AnimationType::ON_HIT]),
    CallFunc::create([=]() {
      GameMapManager::getInstance()->getGameMap()->removeDynamicActor(this);

      shared_ptr<Skill> key(shared_ptr<Skill>(), this); 
      _user->getActiveSkills().erase(key);
    })
  ));

  if (target) {
    bool isFacingRight = _body->GetLinearVelocity().x > 0;
    float knockBackForceX = (isFacingRight) ? 2.0f : -2.0f;
    float knockBackForceY = 1.0f;
    _user->knockBack(target, knockBackForceX, knockBackForceY);
    _user->inflictDamage(target, getDamage());
  }
}


void MagicalMissile::import(const string& jsonFileName) {
  _skillProfile = Skill::Profile(jsonFileName);
}

EventKeyboard::KeyCode MagicalMissile::getHotkey() const {
  return _skillProfile.hotkey;
}

void MagicalMissile::setHotkey(EventKeyboard::KeyCode hotkey) {
  _skillProfile.hotkey = hotkey;
}

bool MagicalMissile::canActivate() {
  return _user->getCharacterProfile().magicka + _skillProfile.deltaMagicka >= 0;
}

void MagicalMissile::activate() {
  // Make sure this instance is only activated once.
  if (_hasActivated) {
    return;
  }

  _hasActivated = true;
  float x = _user->getBody()->GetPosition().x;
  float y = _user->getBody()->GetPosition().y;

  // Modify character's stats.
  _user->getCharacterProfile().magicka += _skillProfile.deltaMagicka;

  shared_ptr<Skill> key(shared_ptr<Skill>(), this);
  auto it = _user->getActiveSkills().find(key);
  if (it != _user->getActiveSkills().end()) {
    auto actor = std::dynamic_pointer_cast<DynamicActor>(*it);
    GameMapManager::getInstance()->getGameMap()->showDynamicActor(std::move(actor), x, y);
  }

  // Set up kinematicBody's moving speed.
  _flyingSpeed = (_user->isFacingRight()) ? 4 : -4;
  _body->SetLinearVelocity({_flyingSpeed, 0});

  if (!_user->isFacingRight()) {
    _launchFxSprite->setFlippedX(true);
    _bodySprite->setFlippedX(true);
  }

  // Play the magical missile body's animation.
  _bodySprite->runAction(Animate::create(_bodyAnimations[AnimationType::FLYING]));

  // Play launch fx animation.
  const b2Vec2& spellUserBodyPos = _user->getBody()->GetPosition();
  x = spellUserBodyPos.x * kPpm;
  y = spellUserBodyPos.y * kPpm;
  float offset = _user->getCharacterProfile().attackRange;
  x += (_user->isFacingRight()) ? offset : -offset;
  _launchFxSprite->setPosition(x, y);

  _launchFxSprite->runAction(Sequence::createWithTwoActions(
    Animate::create(_bodyAnimations[AnimationType::LAUNCH_FX]),
    CallFunc::create([=]() {
      _bodySpritesheet->removeChild(_launchFxSprite, true);
    })
  ));
}


Skill::Profile& MagicalMissile::getSkillProfile() {
  return _skillProfile;
}

const string& MagicalMissile::getName() const {
  return _skillProfile.name;
}

const string& MagicalMissile::getDesc() const {
  return _skillProfile.desc;
}

string MagicalMissile::getIconPath() const {
  return _skillProfile.textureResDir + "/icon.png";
}


void MagicalMissile::defineBody(b2BodyType bodyType,
                                float x,
                                float y,
                                short categoryBits,
                                short maskBits) {
  float spellOffset = _user->getCharacterProfile().attackRange / kPpm;
  spellOffset = (_user->isFacingRight()) ? spellOffset : -spellOffset;

  b2World* world = _user->getBody()->GetWorld();
  b2BodyBuilder bodyBuilder(world);

  _body = bodyBuilder.type(bodyType)
    .position(x + spellOffset, y, 1)
    .buildBody();

  float scaleFactor = Director::getInstance()->getContentScaleFactor();
  b2Vec2 vertices[4];
  vertices[0] = {-10.0f / scaleFactor,  0.0f / scaleFactor};
  vertices[1] = {  0.0f / scaleFactor, -2.0f / scaleFactor};
  vertices[2] = { 10.0f / scaleFactor,  0.0f / scaleFactor};
  vertices[3] = {  0.0f / scaleFactor,  2.0f / scaleFactor};

  _fixtures[0] = bodyBuilder.newPolygonFixture(vertices, 4, kPpm)
    .categoryBits(categoryBits)
    .maskBits(maskBits)
    .setUserData(this)
    .buildFixture();
}

void MagicalMissile::defineTexture(const string& textureResDir, float x, float y) {
  _bodySpritesheet = SpriteBatchNode::create(textureResDir + "/spritesheet.png");

  _bodyAnimations[AnimationType::LAUNCH_FX] = createAnimation(textureResDir, "launch", 5.0f / kPpm);
  _bodyAnimations[AnimationType::FLYING] = createAnimation(textureResDir, "flying", 1.0f / kPpm);
  _bodyAnimations[AnimationType::ON_HIT] = createAnimation(textureResDir, "on_hit", 8.0f / kPpm);

  // Select a frame as default look for this sprite.
  string frameNamePrefix = StaticActor::getLastDirName(textureResDir);
  _launchFxSprite = Sprite::createWithSpriteFrameName(frameNamePrefix + "_launch/0.png");
  _launchFxSprite->setPosition(x, y);
  _bodySprite = Sprite::createWithSpriteFrameName(frameNamePrefix + "_flying/0.png");
  _bodySprite->setPosition(x, y);
  
  _bodySpritesheet->addChild(_launchFxSprite);
  _bodySpritesheet->addChild(_bodySprite);
  _bodySpritesheet->getTexture()->setAliasTexParameters();
}

}  // namespace vigilante
