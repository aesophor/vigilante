// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_ENEMY_H_
#define VIGILANTE_ENEMY_H_

#include <string>
#include <unordered_map>

#include <Box2D/Box2D.h>

#include "Character.h"
#include "Bot.h"

namespace vigilante {

class Enemy : public Character, public Bot {
 public:
  struct Profile final {
    explicit Profile(const std::string& jsonFileName);
    ~Profile() = default;

    struct DroppedItemData {
      int chance;
      int minAmount;
      int maxAmount;
    };

    // <json, {chance, minAmount, maxAmount}>
    std::unordered_map<std::string, DroppedItemData> droppedItems;
  };

  explicit Enemy(const std::string& jsonFileName);
  virtual ~Enemy() = default;

  virtual void update(float delta) override;  // Character
  virtual void showOnMap(float x, float y) override;  // Character
  virtual void import(const std::string& jsonFileName) override;  // Character
  virtual void receiveDamage(Character* source, int damage) override;  // Character

  virtual void act(float delta) override;  // Bot
  virtual void moveToTarget(Character* target) override;  // Bot
  virtual void moveRandomly(float delta, int minMoveDuration, int maxMoveDuration,
                            int minWaitDuration, int maxWaitDuration) override;  // Bot
  virtual void jumpIfStucked(float delta, float checkInterval) override;  // Bot
  virtual void reverseDirection() override;  // Bot

  Enemy::Profile& getEnemyProfile();
  
 private:
  Enemy::Profile _enemyProfile;

  // The following variables are used in Bot::moveRandomly()
  bool _isMovingRight;
  float _moveDuration;
  float _moveTimer;
  float _waitDuration;
  float _waitTimer;

  // The following variables are used in Bot::jumpIfStucked()
  b2Vec2 _lastStoppedPosition;
  float _lastTraveledDistance;
  float _calculateDistanceTimer;
};

}  // namespace vigilante

#endif  // VIGILANTE_ENEMY_H_
