// Copyright (c) 2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_MAP_LIGHTING_H_
#define VIGILANTE_MAP_LIGHTING_H_

#include <list>

#include <axmol.h>

#include "DynamicActor.h"
#include "gameplay/InGameTime.h"
#include "StaticActor.h"

namespace vigilante {

class GameMap;

class Lighting final {
 public:
  Lighting();
  ~Lighting() { clear(); }

  void update();
  void addLightSource(DynamicActor* dynamicActor);
  void addLightSource(StaticActor* staticActor);
  void addLightSource(const float x, const float y);
  void setDarknessOverlaySize(const float width, const float height) const;
  void clear();

  inline ax::Layer* getLayer() const { return _layer; }
  inline void setGameMap(GameMap* gameMap) { _gameMap = gameMap; }
  inline void setAmbientLightLevel(const float level) { _ambientLightLevel = level; }

 private:
  float getBrightnessPercentage(const InGameTime* inGameTime) const;
  void updateAmbientLightLevel(const InGameTime* inGameTime, const float brightnessPercentage);
  void updateParallaxLightLevel(const InGameTime* inGameTime, const float brightnessPercentage);
  void updateLightSources();

  ax::Layer* _layer{};
  ax::RenderTexture* _darknessOverlay{};
  std::list<std::pair<DynamicActor*, ax::Sprite*>> _dynamicLightSources;
  std::list<std::pair<std::pair<float, float>, ax::Sprite*>> _staticLightSources;

  GameMap* _gameMap{};
  float _ambientLightLevel{0.3f};
};

}  // namespace vigilante

#endif  // VIGILANTE_MAP_LIGHTING_H_
