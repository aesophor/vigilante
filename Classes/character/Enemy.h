#ifndef VIGILANTE_ENEMY_H_
#define VIGILANTE_ENEMY_H_

#include <string>
#include <unordered_map>

#include "Box2D/Box2D.h"

#include "Character.h"
#include "Bot.h"

namespace vigilante {

class Enemy : public Character, public Bot {
 public:
  struct Profile {
    Profile(const std::string& jsonFileName);
    virtual ~Profile() = default;

    std::unordered_map<std::string, float> dropItems; // Item*, chance of dropping
  };

  Enemy(const std::string& jsonFileName);
  virtual ~Enemy() = default;

  virtual void update(float delta) override; // Character
  virtual void showOnMap(float x, float y) override; // Character
  virtual void import(const std::string& jsonFileName) override; // Character

  virtual void receiveDamage(Character* source, int damage) override; // Character

  Enemy::Profile& getEnemyProfile();
  
 private:
  Enemy::Profile _enemyProfile;
};

} // namespace vigilante

#endif // VIGILANTE_ENEMY_H_
