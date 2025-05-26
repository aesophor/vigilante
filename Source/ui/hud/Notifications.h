// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_UI_HUD_NOTIFICATIONS_H_
#define REQUIEM_UI_HUD_NOTIFICATIONS_H_

#include "ui/TimedLabelService.h"

namespace requiem {

class Notifications final : public TimedLabelService {
 public:
  Notifications();
  virtual ~Notifications() override = default;
};

}  // namespace requiem

#endif  // REQUIEM_UI_HUD_NOTIFICATIONS_H_
