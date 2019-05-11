#include "Enemy.h"

#include "map/GameMapManager.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/CallbackUtil.h"
#include "util/CategoryBits.h"
#include "util/Constants.h"

using std::string;
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
using vigilante::category_bits::kPlayer;
using vigilante::category_bits::kPortal;
using vigilante::category_bits::kEnemy;
using vigilante::category_bits::kMeleeWeapon;
using vigilante::category_bits::kItem;
using vigilante::category_bits::kGround;
using vigilante::category_bits::kPlatform;
using vigilante::category_bits::kWall;
using vigilante::category_bits::kEnemy;
using vigilante::category_bits::kObject;


namespace vigilante {

Enemy::Enemy(const std::string& name, float x, float y) : Character(name, x, y) {
  short bodyCategoryBits = kEnemy;
  short bodyMaskBits = kPortal | kPlayer | kMeleeWeapon | kItem;
  short feetMaskBits = kGround | kPlatform | kWall;
  short weaponMaskBits = kPlayer | kObject;
  defineBody(b2BodyType::b2_dynamicBody, bodyCategoryBits, bodyMaskBits, feetMaskBits, weaponMaskBits, x, y);
  defineTexture(x, y);
}

Enemy::~Enemy() {
  if (_spritesheet) {
    _spritesheet->release();
  }
}


void Enemy::defineTexture(float x, float y) {
  cocos2d::log("[Enemy] loading textures");
  SpriteFrameCache* frameCache = SpriteFrameCache::getInstance();

  string location = "Texture/Character/Player/sprites/";
//  frameCache->addSpriteFramesWithFile(location + "player.plist");
  _spritesheet = SpriteBatchNode::create(location + "player.png");

  loadAnimation(State::IDLE_SHEATHED, "p_idle_sheathed", 6, 10.0f / kPpm);
  loadAnimation(State::IDLE_UNSHEATHED, "p_idle_unsheathed", 6, 10.0f / kPpm);

  loadAnimation(State::RUNNING_SHEATHED, "p_running_sheathed", 8, 10.0f / kPpm);
  loadAnimation(State::RUNNING_UNSHEATHED, "p_running_unsheathed", 8, 10.0f / kPpm);

  loadAnimation(State::JUMPING_SHEATHED, "p_jumping_sheathed", 5, 10.0f / kPpm);
  loadAnimation(State::JUMPING_UNSHEATHED, "p_jumping_unsheathed", 5, 10.0f / kPpm);

  loadAnimation(State::FALLING_SHEATHED, "p_falling_sheathed", 1, 10.0f / kPpm);
  loadAnimation(State::FALLING_UNSHEATHED, "p_falling_unsheathed", 1, 10.0f / kPpm);

  loadAnimation(State::CROUCHING_SHEATHED, "p_crouching_sheathed", 2, 10.0f / kPpm);
  loadAnimation(State::CROUCHING_UNSHEATHED, "p_crouching_unsheathed", 2, 10.0f / kPpm);

  loadAnimation(State::SHEATHING_WEAPON, "p_weapon_sheathing", 6, 15.0f / kPpm);
  loadAnimation(State::UNSHEATHING_WEAPON, "p_weapon_unsheathing", 6, 15.0f / kPpm);

  loadAnimation(State::ATTACKING, "p_attacking", 8, 5.0f / kPpm);
  loadAnimation(State::KILLED, "p_killed", 6, 24.0f / kPpm);

  // Select a frame as default look of character's sprite.
  _sprite = Sprite::createWithSpriteFrameName("p_idle_sheathed/p_idle_sheathed0.png");
  _sprite->setPosition(x * kPpm, y * kPpm + 7);
  _sprite->setScale(1.3f);

  _spritesheet->addChild(_sprite);
  _spritesheet->getTexture()->setAliasTexParameters(); // disable texture antialiasing

  runAnimation(State::IDLE_SHEATHED);
}

} // namespace vigilante
