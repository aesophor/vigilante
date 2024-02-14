// Copyright (c) 2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_UI_HUD_LOCATION_INFO_H_
#define VIGILANTE_UI_HUD_LOCATION_INFO_H_

#include <string>

#include <axmol.h>
#include <2d/Label.h>

namespace vigilante {

class LocationInfo final {
 public:
  LocationInfo();

  void updateLocationName();

  inline ax::Layer* getLayer() const { return _layer; }

 private:
  ax::Layer* _layer{};
  ax::Label* _locationNameLabel{};
};

}  // namespace vigilante

#endif  // VIGILANTE_UI_HUD_LOCATION_INFO_H_
