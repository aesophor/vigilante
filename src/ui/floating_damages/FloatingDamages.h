// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_FLOATING_DAMAGES_H_
#define VIGILANTE_FLOATING_DAMAGES_H_

#include <deque>
#include <map>
#include <string>

#include <cocos2d.h>

namespace vigilante {

class Character;

class FloatingDamages {
 public:
  static FloatingDamages* getInstance();
  virtual ~FloatingDamages() = default;

  void update(float delta);
  void show(Character* character, int damage);
  inline cocos2d::Layer* getLayer() const { return _layer; }

 private:
  struct DamageLabel {
    DamageLabel(const std::string& text, float lifetime);
    bool operator==(const DamageLabel& other) { return label == other.label; }

    cocos2d::Label* label;
    float lifetime;
    float timer;
  };

  FloatingDamages();

  static inline constexpr float kDeltaX = 0.0f;
  static inline constexpr float kDeltaY = 10.0f;
  static inline constexpr float kMoveUpDuration = .2f;
  static inline constexpr float kFadeDuration = .2f;

  cocos2d::Layer* _layer;
  std::map<Character*, std::deque<FloatingDamages::DamageLabel>> _damageMap;
};

}  // namespace vigilante

#endif  // VIGILANTE_FLOATING_DAMAGES_H_
