// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_FLOATING_DAMAGE_MANAGER_H_
#define VIGILANTE_FLOATING_DAMAGE_MANAGER_H_

#include <map>
#include <deque>
#include <string>

#include "cocos2d.h"

namespace vigilante {

class Character;

class FloatingDamageManager {
 public:
  static FloatingDamageManager* getInstance();
  virtual ~FloatingDamageManager() = default;

  void update(float delta);
  void show(Character* character, int damage);
  cocos2d::Layer* getLayer() const;

 private:
  struct FloatingDamage {
    FloatingDamage(const std::string& text, float lifetime);
    bool operator== (const FloatingDamage& other);
    cocos2d::Label* label;
    float lifetime;
    float timer;
  };

  static FloatingDamageManager* _instance;
  FloatingDamageManager();

  static const float kDeltaX;
  static const float kDeltaY;

  static const float kMoveUpDuration;
  static const float kFadeDuration;

  cocos2d::Layer* _layer;
  std::map<Character*, std::deque<FloatingDamageManager::FloatingDamage>> _damageMap;
};

} // namespace vigilante

#endif // VIGILANTE_FLOATING_DAMAGE_MANAGER_H_
