// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_NOTIFICATION_MANAGER_H_
#define VIGILANTE_NOTIFICATION_MANAGER_H_

#include <deque>
#include <string>

#include "cocos2d.h"
#include "2d/CCLabel.h"

namespace vigilante {

class NotificationManager {
 public:
  static NotificationManager* getInstance();
  virtual ~NotificationManager() = default;

  void update(float delta);
  void show(const std::string& message);
  void setMaxNotificationCount(int maxNotificationCount);

  cocos2d::Layer* getLayer() const;

 private:
  struct Notification {
    Notification(const std::string& text, float lifetime);
    bool operator== (const Notification& other);

    cocos2d::Label* label;
    float lifetime;
    float timer;
  };

  static NotificationManager* _instance;
  NotificationManager();

  static const float kStartingX;
  static const float kStartingY;

  static const float kDeltaX;
  static const float kDeltaY;

  static const float kMoveUpDuration;
  static const float kFadeDuration;

  static const int kDefaultMaxNotificationCount;

  cocos2d::Layer* _layer;
  std::deque<NotificationManager::Notification> _notificationQueue;
  int _maxNotificationCount;
};

} // namespace vigilante

#endif // VIGILANTE_NOTIFICATION_MANAGER_H_
