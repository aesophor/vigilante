#include "Hud.h"

#include "item/Equipment.h"

using std::string;
using std::unique_ptr;
using cocos2d::Layer;
using cocos2d::ui::ImageView;

namespace vigilante {

const string Hud::_kHudLocation = "Texture/UI/HUD/";
const string Hud::_kEquippedWeaponSlotLocation = _kHudLocation + "equipped_weapon_slot.png";
const string Hud::_kBarLeftPaddingLocation = _kHudLocation + "bar_padding_left.png";
const string Hud::_kBarRightPaddingLocation = _kHudLocation + "bar_padding_right.png";
const string Hud::_kHealthBarLocation = _kHudLocation + "health_bar.png";
const string Hud::_kMagickaBarLocation = _kHudLocation + "magicka_bar.png";
const string Hud::_kStaminaBarLocation = _kHudLocation + "stamina_bar.png";
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
      _equippedWeapon(ImageView::create()),
      _equippedWeaponSlot(ImageView::create(_kEquippedWeaponSlotLocation)),
      _healthBar(new StatusBar(_kBarLeftPaddingLocation, _kBarRightPaddingLocation, _kHealthBarLocation, _kBarLength)),
      _magickaBar(new StatusBar(_kBarLeftPaddingLocation, _kBarRightPaddingLocation, _kMagickaBarLocation, _kBarLength)),
      _staminaBar(new StatusBar(_kBarLeftPaddingLocation, _kBarRightPaddingLocation, _kStaminaBarLocation, _kBarLength)) {
  _equippedWeaponSlot->setAnchorPoint({0, 0});
  _equippedWeaponSlot->setPosition({-40, -30});

  _magickaBar->getLayout()->setPositionY(_magickaBar->getLayout()->getPositionY() - 8.0f);
  _staminaBar->getLayout()->setPositionY(_staminaBar->getLayout()->getPositionY() - 16.0f);

  _layer->addChild(_equippedWeaponSlot);
  _layer->addChild(_equippedWeapon);
  _layer->addChild(_healthBar->getLayout());
  _layer->addChild(_magickaBar->getLayout());
  _layer->addChild(_staminaBar->getLayout());
}


void Hud::updateEquippedWeapon() {
  // Update equipped weapon.
  Equipment* weapon = _player->getEquipmentSlots()[Equipment::Type::WEAPON];
  if (weapon) {
    // Replace weapon icon
    _equippedWeapon->loadTexture(weapon->getIconPath());
    _equippedWeapon->setAnchorPoint({0, 0});
    _equippedWeapon->setPosition({-36, -25});
    _equippedWeapon->setScale(_equippedWeaponSlot->getLayoutSize().width / _equippedWeapon->getLayoutSize().width - .5f);
  } else {
    _layer->removeChild(_equippedWeapon);
    _equippedWeapon = ImageView::create();
    _layer->addChild(_equippedWeapon);
  }
}

void Hud::updateStatusBars() {
  // Update status bars.
  _healthBar->update(_player->getHealth(), _player->getFullHealth());
  _magickaBar->update(_player->getMagicka(), _player->getFullMagicka());
  _staminaBar->update(_player->getStamina(), _player->getFullStamina());
}


Layer* Hud::getLayer() const {
  return _layer;
}

void Hud::setPlayer(Player* player) {
  _player = player;
}

} // namespace vigilante
