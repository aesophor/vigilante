// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "Chest.h"

#include "Assets.h"
#include "Audio.h"
#include "Constants.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"
#include "util/B2BodyBuilder.h"
#include "util/JsonUtil.h"
#include "util/StringUtil.h"

using namespace std;
using namespace vigilante::assets;
using namespace vigilante::category_bits;
USING_NS_AX;

namespace vigilante {

namespace {

constexpr int kChestNumAnimations = 0;
constexpr int kChestNumFixtures = 2;

constexpr auto kItemCategoryBits = kInteractable;
constexpr auto kItemMaskBits = kGround | kPlatform | kWall;

}  // namespace

Chest::Chest(const string& tmxMapFilePath,
             const int chestId,
             const string& itemJsons)
    : DynamicActor{kChestNumAnimations, kChestNumFixtures},
      _tmxMapFilePath{tmxMapFilePath},
      _chestId{chestId},
      _itemJsons{string_util::split(itemJsons)} {
  constexpr auto kType = GameMap::OpenableObjectType::CHEST;
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  _isOpened = gmMgr->isOpened(tmxMapFilePath, kType, chestId);
}

bool Chest::showOnMap(float x, float y) {
  if (_isShownOnMap) {
    return false;
  }

  _isShownOnMap = true;

  defineBody(b2BodyType::b2_dynamicBody, x, y,
             kItemCategoryBits,
             kItemMaskBits);

  if (!_isOpened) {
    _bodySprite = Sprite::create("Texture/interactable_object/chest/chest_close.png");
  } else {
    _bodySprite = Sprite::create("Texture/interactable_object/chest/chest_open.png");
  }
  _bodySprite->getTexture()->setAliasTexParameters();

  _node->addChild(_bodySprite, graphical_layers::kChest);

  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  gmMgr->getLayer()->addChild(_node, graphical_layers::kChest);

  return true;
}

void Chest::defineBody(b2BodyType bodyType,
                       float x,
                       float y,
                       short categoryBits,
                       short maskBits) {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  B2BodyBuilder bodyBuilder(gmMgr->getWorld());

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
  gmMgr->setOpened(_tmxMapFilePath, kType, _chestId, true);

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
