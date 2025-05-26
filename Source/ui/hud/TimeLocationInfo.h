// Copyright (c) 2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_UI_HUD_TIME_LOCATION_INFO_H_
#define REQUIEM_UI_HUD_TIME_LOCATION_INFO_H_

#include <string>

#include <axmol.h>
#include <2d/Label.h>

namespace requiem {

class TimeLocationInfo final {
 public:
  TimeLocationInfo();

  void update();

  inline ax::Layer* getLayer() const { return _layer; }

 private:
  ax::Layer* _layer{};
  ax::Label* _locationNameLabel{};
};

}  // namespace requiem

#endif  // REQUIEM_UI_HUD_TIME_LOCATION_INFO_H_
