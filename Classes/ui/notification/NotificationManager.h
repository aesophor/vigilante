// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_NOTIFICATION_MANAGER_H_
#define VIGILANTE_NOTIFICATION_MANAGER_H_

#include "ui/TimedLabelService.h"

namespace vigilante {

class NotificationManager : public TimedLabelService {
 public:
  static NotificationManager* getInstance();
  virtual ~NotificationManager() = default;

 private:
  static NotificationManager* _instance;
  NotificationManager();
};

} // namespace vigilante

#endif // VIGILANTE_NOTIFICATION_MANAGER_H_
