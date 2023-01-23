// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Chest.h"

#include "Assets.h"
#include "Audio.h"
#include "Constants.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "util/box2d/b2BodyBuilder.h"
#include "util/JsonUtil.h"
#include "util/StringUtil.h"

#define CHEST_NUM_ANIMATIONS 0
#define CHEST_NUM_FIXTURES 2

#define ITEM_CATEGORY_BITS kInteractable
#define ITEM_MASK_BITS kGround | kPlatform | kWall

using namespace std;
using namespace vigilante::assets;
using namespace vigilante::category_bits;
USING_NS_CC;

namespace vigilante {

Chest::Chest(const string& tmxMapFileName,
             const int chestId,
             const string& itemJsons)
    : DynamicActor(CHEST_NUM_ANIMATIONS, CHEST_NUM_FIXTURES),
      _tmxMapFileName(tmxMapFileName),
      _chestId(chestId),
      _itemJsons(string_util::split(itemJsons)),
      _isOpened(),
      _hintBubbleFxSprite() {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  constexpr auto kType = GameMap::OpenableObjectType::CHEST;
  _isOpened = gmMgr->isOpened(tmxMapFileName, kType, chestId);
}

bool Chest::showOnMap(float x, float y) {
  if (_isShownOnMap) {
    return false;
  }

  _isShownOnMap = true;

  defineBody(b2BodyType::b2_dynamicBody,
             x,
             y,
             ITEM_CATEGORY_BITS,
             ITEM_MASK_BITS);

  if (!_isOpened) {
    _bodySprite = Sprite::create("Texture/interactable_object/chest/chest_close.png");
  } else {
    _bodySprite = Sprite::create("Texture/interactable_object/chest/chest_open.png");
  }
  _bodySprite->getTexture()->setAliasTexParameters();

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  gmMgr->getLayer()->addChild(_bodySprite, graphical_layers::kChest);

  return true;
}

void Chest::defineBody(b2BodyType bodyType,
                       float x,
                       float y,
                       short categoryBits,
                       short maskBits) {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  b2BodyBuilder bodyBuilder(gmMgr->getWorld());

  _body = bodyBuilder.type(bodyType)
    .position(x, y, kPpm)
    .buildBody();

  bodyBuilder.newRectangleFixture(16 / 2, 16 / 2, kPpm)
    .categoryBits(categoryBits)
    .maskBits(maskBits)
    .setUserData(this)
    .buildFixture();

  bodyBuilder.newRectangleFixture(16 / 2, 16 / 2, kPpm)
    .categoryBits(kInteractable)
    .maskBits(kFeet)
    .setSensor(true)
    .setUserData(static_cast<Interactable*>(this))
    .buildFixture();
}

void Chest::onInteract(Character*) {
  if (_isOpened) {
    return;
  }

  _isOpened = true;
  _bodySprite->setTexture("Texture/interactable_object/chest/chest_open.png");
  _bodySprite->getTexture()->setAliasTexParameters();

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  for (const auto& item : _itemJsons) {
    float x = _body->GetPosition().x;
    float y = _body->GetPosition().y;
    gmMgr->getGameMap()->createItem(item, x * kPpm, y * kPpm);
  }

  constexpr auto kType = GameMap::OpenableObjectType::CHEST;
  gmMgr->setOpened(_tmxMapFileName, kType, _chestId, true);

  Audio::the().playSfx(kSfxChestOpened);
}

void Chest::showHintUI() {
  if (_isOpened) {
    return;
  }

  createHintBubbleFx();

  auto controlHints = SceneManager::the().getCurrentScene<GameScene>()->getControlHints();
  controlHints->insert({EventKeyboard::KeyCode::KEY_CAPITAL_E}, "Open");
}

void Chest::hideHintUI() {
  removeHintBubbleFx();

  auto controlHints = SceneManager::the().getCurrentScene<GameScene>()->getControlHints();
  controlHints->remove({EventKeyboard::KeyCode::KEY_CAPITAL_E});
}

void Chest::createHintBubbleFx() {
  if (_hintBubbleFxSprite) {
    removeHintBubbleFx();
  }

  if (_isOpened) {
    return;
  }

  auto fxMgr = SceneManager::the().getCurrentScene<GameScene>()->getFxManager();
  _hintBubbleFxSprite = fxMgr->createHintBubbleFx(_body, "dialogue_available");
}

void Chest::removeHintBubbleFx() {
  if (!_hintBubbleFxSprite) {
    return;
  }

  auto fxMgr = SceneManager::the().getCurrentScene<GameScene>()->getFxManager();
  fxMgr->removeFx(_hintBubbleFxSprite);
  _hintBubbleFxSprite = nullptr;
}

}  // namespace vigilante
