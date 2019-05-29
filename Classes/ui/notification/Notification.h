#ifndef VIGILANTE_NOTIFICATION_H_
#define VIGILANTE_NOTIFICATION_H_

#include <deque>
#include <string>

#include "cocos2d.h"
#include "2d/CCLabel.h"

namespace vigilante {

class Notification {
 public:
  static Notification* getInstance();
  virtual ~Notification() = default;

  void update(float delta);
  void show(const std::string& message);
  void setMaxNotificationCount(int maxNotificationCount);

  cocos2d::Layer* getLayer() const;

 private:
  struct NotificationLabel {
    NotificationLabel(const std::string& text, float lifetime);
    bool operator== (const NotificationLabel& other);

    cocos2d::Label* label;
    float lifetime;
    float timer;
  };

  static Notification* _instance;
  Notification();

  static const float kStartingX;
  static const float kStartingY;

  static const float kDeltaX;
  static const float kDeltaY;

  static const float kMoveUpDuration;
  static const float kFadeDuration;

  static const int kDefaultMaxNotificationCount;

  cocos2d::Layer* _layer;
  std::deque<Notification::NotificationLabel> _notificationQueue;
  int _maxNotificationCount;
};

} // namespace vigilante

#endif // VIGILANTE_NOTIFICATION_H_
