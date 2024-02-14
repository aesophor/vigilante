// Copyright (c) 2023-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_MAP_PARALLAX_BACKGROUND_H_
#define VIGILANTE_MAP_PARALLAX_BACKGROUND_H_

#include <filesystem>
#include <map>
#include <string>

#include <axmol.h>

namespace vigilante {

class InfiniteParallaxNode : public ax::Node {
 public:
  struct ParallaxLayerData {
    ax::Vec2 ratio;
    ax::Vec2 position;
    ax::Vec2 scale;
    ax::Sprite *spriteA{};
    ax::Sprite *spriteB{};
  };

  CREATE_FUNC(InfiniteParallaxNode);

  virtual bool init();
  virtual void setPosition(const ax::Vec2 &position) {
    _speed = position - _position;
    _position = position;
  }
  virtual const ax::Vec2& getPosition() const { return _position; }

  void addLayer(const std::string &filePath,
                const int z,
                const ax::Vec2& parallaxRatio,
                const ax::Vec2& position,
                const ax::Vec2& scale);
  void update(const float delta);

 private:
  ax::Size _visibleSize;
  ax::Vec2 _speed;
  std::unordered_map<std::string, ParallaxLayerData> _layerData;
  ax::Vec2 _prevGameCameraPos;
};

class ParallaxBackground final {
 public:
  void update(const float delta);
  bool load(const std::filesystem::path& bgDirPath, const float bgScale);

  inline InfiniteParallaxNode* getParallaxNode() const { return _parallaxNode; }

 private:
  InfiniteParallaxNode* _parallaxNode{};
};

}  // namespace vigilante

#endif  // VIGILANTE_MAP_PARALLAX_BACKGROUND_H_
