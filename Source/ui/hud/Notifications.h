// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_UI_HUD_NOTIFICATIONS_H_
#define VIGILANTE_UI_HUD_NOTIFICATIONS_H_

#include "ui/TimedLabelService.h"

namespace vigilante {

class Notifications final : public TimedLabelService {
 public:
  Notifications();
  virtual ~Notifications() override = default;
};

}  // namespace vigilante

#endif  // VIGILANTE_UI_HUD_NOTIFICATIONS_H_
