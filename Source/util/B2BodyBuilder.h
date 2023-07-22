// Copyright (c) 2018-2023 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_B2_BODY_BUILDER_H_
#define VIGILANTE_B2_BODY_BUILDER_H_

#include <memory>

#include <box2d/box2d.h>

namespace vigilante {

class B2BodyBuilder final {
 public:
  explicit B2BodyBuilder(b2World* world) : _world{world} {}
  explicit B2BodyBuilder(b2Body* body) : _world{body->GetWorld()}, _body{body} {}
  virtual ~B2BodyBuilder() = default;

  B2BodyBuilder& type(b2BodyType bodyType);
  B2BodyBuilder& position(float x, float y, float ppm);
  B2BodyBuilder& position(b2Vec2 position, float ppm);
  b2Body* buildBody();

  B2BodyBuilder& newRectangleFixture(float hx, float hy, float ppm);
  B2BodyBuilder& newPolygonFixture(const b2Vec2* vertices, size_t count, float ppm);
  B2BodyBuilder& newPolylineFixture(const b2Vec2* vertices, size_t count, float ppm);
  B2BodyBuilder& newEdgeShapeFixture(const b2Vec2& vertex1, const b2Vec2& vertex2, float ppm);
  B2BodyBuilder& newCircleFixture(const b2Vec2& centerPos, int radius, float ppm);

  B2BodyBuilder& categoryBits(short categoryBits);
  B2BodyBuilder& maskBits(short maskBits);
  B2BodyBuilder& setSensor(bool isSensor);
  B2BodyBuilder& friction(float friction);
  B2BodyBuilder& density(float density);
  B2BodyBuilder& restitution(float restitution);
  B2BodyBuilder& setUserData(void* userData);
  b2Fixture* buildFixture();

 private:
  b2World* _world{};
  b2Body* _body{};
  b2Fixture* _fixture{};

  b2BodyDef _bdef{};
  b2FixtureDef _fdef{};
  std::unique_ptr<b2Shape> _shape;
  void* _userData{};
};

}  // namespace vigilante

#endif  // VIGILANTE_B2_BODY_BUILDER_H_
