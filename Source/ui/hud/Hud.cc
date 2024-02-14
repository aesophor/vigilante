// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "Hud.h"

#include "Assets.h"
#include "Constants.h"
#include "character/Player.h"
#include "item/Equipment.h"
#include "scene/GameScene.h"
#include "scene/SceneManager.h"

#define HUD_X 75
#define HUD_Y ax::Director::getInstance()->getWinSize().height - 40

using namespace std;
using namespace vigilante::assets;
USING_NS_AX;

namespace vigilante {

Hud::Hud()
    : _layer{Layer::create()},
      _healthBar{make_unique<StatusBar>(kBarLeftPadding, kBarRightPadding, kHealthBar, _kBarLength)},
      _magickaBar{make_unique<StatusBar>(kBarLeftPadding, kBarRightPadding, kMagickaBar, _kBarLength)},
      _staminaBar{make_unique<StatusBar>(kBarLeftPadding, kBarRightPadding, kStaminaBar, _kBarLength)},
      _equippedWeaponBg{ui::ImageView::create(string{kEquippedWeaponBg})},
      _equippedWeapon{ui::ImageView::create()},
      _equippedWeaponDescBg{ui::ImageView::create(string{kEquippedWeaponDescBg})},
      _equippedWeaponDesc{Label::createWithTTF("", string{kRegularFont}, kRegularFontSize)} {
  _equippedWeaponBg->setPosition({-20, -15});
  _equippedWeaponDescBg->setPosition({33, -25});

  _equippedWeaponDesc->getFontAtlas()->setAliasTexParameters();
  _equippedWeaponDesc->setAnchorPoint({0, 0});
  _equippedWeaponDesc->setPosition({5.0f, -30.f});

  _magickaBar->getLayout()->setPositionY(_magickaBar->getLayout()->getPositionY() - 6.0f);
  _staminaBar->getLayout()->setPositionY(_staminaBar->getLayout()->getPositionY() - 12.0f);

  _layer->setPosition(HUD_X, HUD_Y);
  _layer->addChild(_equippedWeaponBg);
  _layer->addChild(_equippedWeapon);
  _layer->addChild(_healthBar->getLayout());
  _layer->addChild(_magickaBar->getLayout());
  _layer->addChild(_staminaBar->getLayout());
  _layer->addChild(_equippedWeaponDescBg);
  _layer->addChild(_equippedWeaponDesc);
}

void Hud::updateEquippedWeapon() {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  Equipment* weapon = gmMgr->getPlayer()->getEquipmentSlots()[Equipment::Type::WEAPON];

  if (weapon) {
    // Replace weapon icon
    _equippedWeapon->loadTexture(weapon->getIconPath());
    _equippedWeapon->setPosition(_equippedWeaponBg->getPosition());
    _equippedWeapon->setScale(1.5);
    // Update weapon desc label
    _equippedWeaponDesc->setString(weapon->getItemProfile().name);
  } else {
    _equippedWeapon->loadTexture(string{assets::kEmptyImage});
    _equippedWeaponDesc->setString("");
  }
}

void Hud::updateStatusBars() {
  auto gmMgr = SceneManager::the().getCurrentScene<GameScene>()->getGameMapManager();
  Character::Profile& profile = gmMgr->getPlayer()->getCharacterProfile();

  _healthBar->update(profile.health, profile.fullHealth);
  _magickaBar->update(profile.magicka, profile.fullMagicka);
  _staminaBar->update(profile.stamina, profile.fullStamina);
}

}  // namespace vigilante
