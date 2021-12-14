// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "Hud.h"

#include "AssetManager.h"
#include "Constants.h"
#include "character/Player.h"
#include "item/Equipment.h"
#include "map/GameMapManager.h"

#define HUD_X 75
#define HUD_Y cocos2d::Director::getInstance()->getWinSize().height - 40

using std::string;
using std::unique_ptr;
using cocos2d::Layer;
using cocos2d::Label;
using cocos2d::ui::ImageView;
using vigilante::asset_manager::kRegularFont;
using vigilante::asset_manager::kRegularFontSize;
using vigilante::asset_manager::kBarLeftPadding;
using vigilante::asset_manager::kBarRightPadding;
using vigilante::asset_manager::kHealthBar;
using vigilante::asset_manager::kMagickaBar;
using vigilante::asset_manager::kStaminaBar;
using vigilante::asset_manager::kEquippedWeaponBg;
using vigilante::asset_manager::kEquippedWeaponDescBg;

namespace vigilante {

const float Hud::_kBarLength = 75.0f;

Hud* Hud::getInstance() {
  static Hud instance;
  return &instance;
}

Hud::Hud()
    : _layer(Layer::create()),
      _healthBar(std::make_unique<StatusBar>(kBarLeftPadding, kBarRightPadding, kHealthBar, _kBarLength)),
      _magickaBar(std::make_unique<StatusBar>(kBarLeftPadding, kBarRightPadding, kMagickaBar, _kBarLength)),
      _staminaBar(std::make_unique<StatusBar>(kBarLeftPadding, kBarRightPadding, kStaminaBar, _kBarLength)),
      _equippedWeaponBg(ImageView::create(kEquippedWeaponBg)),
      _equippedWeapon(ImageView::create()),
      _equippedWeaponDescBg(ImageView::create(kEquippedWeaponDescBg)),
      _equippedWeaponDesc(Label::createWithTTF("", kRegularFont, kRegularFontSize)) {
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
  Equipment* weapon = GameMapManager::getInstance()->getPlayer()->getEquipmentSlots()[Equipment::Type::WEAPON];

  if (weapon) {
    // Replace weapon icon
    _equippedWeapon->loadTexture(weapon->getIconPath());
    _equippedWeapon->setPosition(_equippedWeaponBg->getPosition());
    _equippedWeapon->setScale(1.5);
    // Update weapon desc label
    _equippedWeaponDesc->setString(weapon->getItemProfile().name);
  } else {
    _equippedWeapon->loadTexture(asset_manager::kEmptyImage);
    _equippedWeaponDesc->setString("");
  }
}

void Hud::updateStatusBars() {
  Character::Profile& profile = GameMapManager::getInstance()->getPlayer()->getCharacterProfile();

  _healthBar->update(profile.health, profile.fullHealth);
  _magickaBar->update(profile.magicka, profile.fullMagicka);
  _staminaBar->update(profile.stamina, profile.fullStamina);
}


Layer* Hud::getLayer() const {
  return _layer;
}

}  // namespace vigilante
