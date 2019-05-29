#ifndef VIGILANTE_FLOATING_DAMAGE_H_
#define VIGILANTE_FLOATING_DAMAGE_H_

#include <map>
#include <deque>
#include <string>

#include "cocos2d.h"

namespace vigilante {

class Character;

class FloatingDamage {
 public:
  static FloatingDamage* getInstance();
  virtual ~FloatingDamage() = default;

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

  static FloatingDamage* _instance;
  FloatingDamage();

  static const float kDeltaX;
  static const float kDeltaY;

  static const float kMoveUpDuration;
  static const float kFadeDuration;

  cocos2d::Layer* _layer;
  std::map<Character*, std::deque<FloatingDamage::DamageLabel>> _damageMap;
};

} // namespace vigilante

#endif // VIGILANTE_FLOATING_DAMAGE_MANAGER_H_
