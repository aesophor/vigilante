#include "Hud.h"

#include "GameAssetManager.h"
#include "item/Equipment.h"

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

Hud* Hud::_instance = nullptr;

Hud* Hud::getInstance() {
  if (!_instance) {
    _instance = new Hud();
  }
  return _instance;
}

Hud::Hud()
    : _layer(Layer::create()),
      _player(),
      _healthBar(new StatusBar(kBarLeftPadding, kBarRightPadding, kHealthBar, _kBarLength)),
      _magickaBar(new StatusBar(kBarLeftPadding, kBarRightPadding, kMagickaBar, _kBarLength)),
      _staminaBar(new StatusBar(kBarLeftPadding, kBarRightPadding, kStaminaBar, _kBarLength)),
      _equippedWeaponBg(ImageView::create(kEquippedWeaponBg)),
      _equippedWeapon(ImageView::create()),
      _equippedWeaponDescBg(ImageView::create(kEquippedWeaponDescBg)),
      _equippedWeaponDesc(Label::createWithTTF("", kRegularFont, kRegularFontSize)) {
  _equippedWeaponBg->setAnchorPoint({0, 0});
  _equippedWeaponBg->setPosition({-40, -31});

  _equippedWeaponDescBg->setAnchorPoint({0, 0});
  _equippedWeaponDescBg->setPosition({0, -31});

  _equippedWeaponDesc->setAnchorPoint({0, 0});
  _equippedWeaponDesc->getFontAtlas()->setAliasTexParameters();
  _equippedWeaponDesc->setPosition({5.0f, -30.f});

  _magickaBar->getLayout()->setPositionY(_magickaBar->getLayout()->getPositionY() - 6.0f);
  _staminaBar->getLayout()->setPositionY(_staminaBar->getLayout()->getPositionY() - 12.0f);

  _layer->addChild(_equippedWeaponBg);
  _layer->addChild(_equippedWeapon);
  _layer->addChild(_healthBar->getLayout());
  _layer->addChild(_magickaBar->getLayout());
  _layer->addChild(_staminaBar->getLayout());
  _layer->addChild(_equippedWeaponDescBg);
  _layer->addChild(_equippedWeaponDesc);
}


void Hud::updateEquippedWeapon() {
  // Update equipped weapon.
  Equipment* weapon = _player->getEquipmentSlots()[Equipment::Type::WEAPON];
  if (weapon) {
    // Replace weapon icon
    _equippedWeapon->loadTexture(weapon->getIconPath());
    _equippedWeapon->setAnchorPoint({0, 0});
    _equippedWeapon->setPosition({-36, -27});
    _equippedWeapon->setScale(_equippedWeaponBg->getLayoutSize().width / _equippedWeapon->getLayoutSize().width - .5f);
    // Update weapon desc label
    _equippedWeaponDesc->setString(weapon->getItemProfile().name);
  } else {
    _equippedWeapon->loadTexture(asset_manager::kEmptyItemIcon);
    _equippedWeaponDesc->setString("");
  }
}

void Hud::updateStatusBars() {
  // Update status bars.
  Character::Profile& profile = _player->getCharacterProfile();
  _healthBar->update(profile.health, profile.fullHealth);
  _magickaBar->update(profile.magicka, profile.fullMagicka);
  _staminaBar->update(profile.stamina, profile.fullStamina);
}


Layer* Hud::getLayer() const {
  return _layer;
}

void Hud::setPlayer(Player* player) {
  _player = player;
}

} // namespace vigilante
