// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "FloatingDamages.h"

#include "Assets.h"
#include "Constants.h"
#include "character/Character.h"
#include "character/Player.h"
#include "character/Npc.h"
#include "ui/Colorscheme.h"

using namespace std;
using namespace vigilante::assets;
USING_NS_CC;

namespace vigilante {

FloatingDamages* FloatingDamages::getInstance() {
  static FloatingDamages instance;
  return &instance;
}

FloatingDamages::FloatingDamages() : _layer(Layer::create()) {}

void FloatingDamages::update(float delta) {
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

void FloatingDamages::show(Character* character, int damage) {
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
  if (dynamic_cast<Player*>(character) || dynamic_cast<Npc*>(character)->isInPlayerParty()) {
    dmg.label->setTextColor(colorscheme::kRed);
  }
  const auto& characterPos = character->getBody()->GetPosition();
  float x = characterPos.x * kPpm;
  float y = characterPos.y * kPpm + 15;
  dmg.label->setPosition(x, y);
  dmg.label->runAction(MoveBy::create(kMoveUpDuration, {kDeltaX, kDeltaY}));

  _damageMap[character].push_back(dmg);
  _layer->addChild(dmg.label);
}

FloatingDamages::DamageLabel::DamageLabel(const string& text, float lifetime)
    : label(Label::createWithTTF(text, kRegularFont, kRegularFontSize)),
      lifetime(lifetime),
      timer() {
  label->getFontAtlas()->setAliasTexParameters();
}

}  // namespace vigilante
