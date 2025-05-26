// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_UI_HUD_FLOATING_DAMAGES_H_
#define REQUIEM_UI_HUD_FLOATING_DAMAGES_H_

#include <deque>
#include <map>
#include <string>

#include <axmol.h>

namespace requiem {

class Character;

class FloatingDamages final {
 public:
  FloatingDamages();

  void update(const float delta);
  void show(Character* character, int damage);
  inline ax::Layer* getLayer() const { return _layer; }

 private:
  struct DamageLabel {
    DamageLabel(const std::string& text, const float lifetime);
    bool operator==(const DamageLabel& other) { return label == other.label; }

    ax::Label* label;
    float lifetime;
    float timer{};
  };

  static inline constexpr float kDeltaX = 0.0f;
  static inline constexpr float kDeltaY = 10.0f;
  static inline constexpr float kMoveUpDuration = .2f;
  static inline constexpr float kFadeDuration = .2f;

  ax::Layer* _layer;
  std::map<Character*, std::deque<FloatingDamages::DamageLabel>> _damageMap;
};

}  // namespace requiem

#endif  // REQUIEM_UI_HUD_FLOATING_DAMAGES_H_
