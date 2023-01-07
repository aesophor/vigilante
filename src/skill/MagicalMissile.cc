// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "MagicalMissile.h"

#include <functional>
#include <memory>

#include "AudioManager.h"
#include "Constants.h"
#include "character/Character.h"
#include "map/GameMapManager.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/Logger.h"

#define MAGICAL_MISSILE_NUM_ANIMATIONS MagicalMissile::AnimationType::SIZE
#define MAGICAL_MISSILE_NUM_FIXTURES 1

#define MAGICAL_MISSILE_CATEOGRY_BITS kProjectile
#define MAGICAL_MISSILE_MASK_BITS kPlayer | kEnemy | kWall

using namespace std;
using namespace vigilante::category_bits;
USING_NS_CC;

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
      _user->removeActiveSkill(this);
    })
  ));

  if (target) {
    bool isFacingRight = _body->GetLinearVelocity().x > 0;
    float knockBackForceX = (isFacingRight) ? 2.0f : -2.0f;
    float knockBackForceY = 1.0f;
    _user->knockBack(target, knockBackForceX, knockBackForceY);
    _user->inflictDamage(target, getDamage());
  }

  // Play sound effect.
  AudioManager::getInstance()->playSfx(_skillProfile.sfxHit);
}

void MagicalMissile::import(const string& jsonFileName) {
  _skillProfile = Skill::Profile(jsonFileName);
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

  shared_ptr<Skill> activeCopy = _user->getActiveSkill(this);
  assert(activeCopy);
  auto actor = std::dynamic_pointer_cast<DynamicActor>(activeCopy);
  GameMapManager::getInstance()->getGameMap()->showDynamicActor(std::move(actor), x, y);

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
