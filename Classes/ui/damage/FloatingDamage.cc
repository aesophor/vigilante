#include "FloatingDamage.h"

#include "Constants.h"
#include "GameAssetManager.h"
#include "character/Character.h"

using std::map;
using std::deque;
using std::string;
using cocos2d::Layer;
using cocos2d::Label;
using cocos2d::MoveBy;
using cocos2d::FadeOut;
using cocos2d::CallFunc;
using cocos2d::Sequence;
using vigilante::kPpm;
using vigilante::asset_manager::kRegularFont;
using vigilante::asset_manager::kRegularFontSize;

namespace vigilante {

FloatingDamage* FloatingDamage::_instance = nullptr;

const float FloatingDamage::kDeltaX = 0.0f;
const float FloatingDamage::kDeltaY = 10.0f;

const float FloatingDamage::kMoveUpDuration = .2f;
const float FloatingDamage::kFadeDuration = .2f;

FloatingDamage* FloatingDamage::getInstance() {
  if (!_instance) {
    _instance = new FloatingDamage();
  }
  return _instance;
}

FloatingDamage::FloatingDamage() : _layer(Layer::create()) {}


void FloatingDamage::update(float delta) {
  deque<map<Character*, deque<DamageLabel>>::iterator> trash;

  auto it = _damageMap.begin();
  for (auto& characterDmgs : _damageMap) { // map<Character*, deque<FloatingDamage>>
    for (auto& dmg : characterDmgs.second) { // FloatingDamage
      dmg.timer += delta;

      if (dmg.timer >= dmg.lifetime) {
        dmg.label->runAction(Sequence::createWithTwoActions(
          FadeOut::create(kFadeDuration),
          CallFunc::create([=]() {
            // After the label fully fades out, remove the label from _layer.
            _layer->removeChild(dmg.label);
          })
        ));
        // Also remove the FloatingDamage object from _damageMap.
        auto& q = _damageMap[characterDmgs.first];
        q.erase(std::remove(q.begin(), q.end(), dmg), q.end());

        if (q.empty()) {
          trash.push_back(it);
        }
      }
    }
    it++;
  }

  // Clean up unsued deques in _damageMap.
  for (auto it : trash) {
    _damageMap.erase(it);
  }
}

void FloatingDamage::show(Character* character, int damage) {
  // If _damageMap does not have a deque of FloatingDamage objects for this character,
  // then initialize it.
  if (_damageMap.find(character) == _damageMap.end()) {
    _damageMap[character] = {};
  }

  // Move up the previous floating damage labels owned by this character.
  for (auto& floatingDamage : _damageMap[character]) {
    floatingDamage.label->runAction(MoveBy::create(kMoveUpDuration, {kDeltaX, kDeltaY}));
  }

  // Display the new floating damage label.
  DamageLabel dmg(std::to_string(damage), 1.5f);
  const auto& characterPos = character->getBody()->GetPosition();
  float x = characterPos.x * kPpm;
  float y = characterPos.y * kPpm + 15;
  dmg.label->setPosition(x, y);
  dmg.label->runAction(MoveBy::create(kMoveUpDuration, {kDeltaX, kDeltaY}));

  _damageMap[character].push_back(dmg);
  _layer->addChild(dmg.label);
}

Layer* FloatingDamage::getLayer() const {
  return _layer;
}


FloatingDamage::DamageLabel::DamageLabel(const string& text, float lifetime)
    : label(Label::createWithTTF(text, kRegularFont, kRegularFontSize)),
      lifetime(lifetime),
      timer() {
  label->getFontAtlas()->setAliasTexParameters();
}

bool FloatingDamage::DamageLabel::operator== (const DamageLabel& other) {
  return this->label == other.label;
}

} // namespace vigilante
