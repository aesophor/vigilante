// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_NOTIFICATIONS_H_
#define VIGILANTE_NOTIFICATIONS_H_

#include "ui/TimedLabelService.h"

namespace vigilante {

class Notifications : public TimedLabelService {
 public:
  static Notifications* getInstance();
  virtual ~Notifications() = default;

 private:
  Notifications();
};

} // namespace vigilante

#endif // VIGILANTE_NOTIFICATIONS_H_
