// Copyright (c) 2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_MAP_LIGHTING_H_
#define VIGILANTE_MAP_LIGHTING_H_

#include <list>

#include <axmol.h>

#include "DynamicActor.h"
#include "StaticActor.h"

namespace vigilante {

class Lighting final {
 public:
  Lighting();
  ~Lighting() { clear(); }

  void update();
  void addLightSource(DynamicActor* dynamicActor);
  void addLightSource(StaticActor* staticActor);
  void setDarknessOverlaySize(const float width, const float height) const;
  void clear();

  inline ax::Layer* getLayer() const { return _layer; }
  inline void setAmbientLightLevel(const float level) { _ambientLightLevel = level; }

 private:
  ax::Layer* _layer{};
  ax::RenderTexture* _darknessOverlay{};
  std::list<std::pair<DynamicActor*, ax::Sprite*>> _dynamicLightSources;
  std::list<std::pair<StaticActor*, ax::Sprite*>> _staticLightSources;

  float _ambientLightLevel{0.3f};
};

}  // namespace vigilante

#endif  // VIGILANTE_MAP_LIGHTING_H_
