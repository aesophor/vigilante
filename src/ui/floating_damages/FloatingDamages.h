// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_FLOATING_DAMAGES_H_
#define VIGILANTE_FLOATING_DAMAGES_H_

#include <map>
#include <deque>
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
  cocos2d::Layer* getLayer() const;

 private:
  struct DamageLabel {
    DamageLabel(const std::string& text, float lifetime);
    bool operator== (const DamageLabel& other);
    cocos2d::Label* label;
    float lifetime;
    float timer;
  };

  static FloatingDamages* _instance;
  FloatingDamages();

  static const float kDeltaX;
  static const float kDeltaY;

  static const float kMoveUpDuration;
  static const float kFadeDuration;

  cocos2d::Layer* _layer;
  std::map<Character*, std::deque<FloatingDamages::DamageLabel>> _damageMap;
};

} // namespace vigilante

#endif // VIGILANTE_FLOATING_DAMAGES_H_
