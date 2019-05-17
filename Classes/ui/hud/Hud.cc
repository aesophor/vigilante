#include "Hud.h"

#include "GameAssetManager.h"
#include "item/Equipment.h"

using std::string;
using std::unique_ptr;
using cocos2d::Layer;
using cocos2d::Label;
using cocos2d::ui::ImageView;

namespace vigilante {

const string Hud::_kHud = "Texture/UI/HUD/";

const string Hud::_kBarLeftPadding = _kHud + "bar_padding_left.png";
const string Hud::_kBarRightPadding = _kHud + "bar_padding_right.png";
const string Hud::_kHealthBar = _kHud + "health_bar.png";
const string Hud::_kMagickaBar = _kHud + "magicka_bar.png";
const string Hud::_kStaminaBar = _kHud + "stamina_bar.png";
const float Hud::_kBarLength = 75.0f;

const string Hud::_kEquippedWeaponBg = _kHud + "equipped_weapon_slot.png";
const string Hud::_kEquippedWeaponDescBg = _kHud + "item_desc.png";
const string Hud::_kFont = "Font/HeartbitXX.ttf";


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
      _healthBar(new StatusBar(_kBarLeftPadding, _kBarRightPadding, _kHealthBar, _kBarLength)),
      _magickaBar(new StatusBar(_kBarLeftPadding, _kBarRightPadding, _kMagickaBar, _kBarLength)),
      _staminaBar(new StatusBar(_kBarLeftPadding, _kBarRightPadding, _kStaminaBar, _kBarLength)),
      _equippedWeaponBg(ImageView::create(_kEquippedWeaponBg)),
      _equippedWeapon(ImageView::create()),
      _equippedWeaponDescBg(ImageView::create(_kEquippedWeaponDescBg)),
      _equippedWeaponDesc(Label::createWithTTF("", _kFont, 16)) {
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
    _equippedWeapon->setPosition({-36, -25});
    _equippedWeapon->setScale(_equippedWeaponBg->getLayoutSize().width / _equippedWeapon->getLayoutSize().width - .5f);
    // Update weapon desc label
    _equippedWeaponDesc->setString(weapon->getName());
  } else {
    _equippedWeapon->loadTexture(asset_manager::kEmptyItemIcon);
    _equippedWeaponDesc->setString("");
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
