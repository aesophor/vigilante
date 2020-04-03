// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_B2_BODY_BUILDER_H_
#define VIGILANTE_B2_BODY_BUILDER_H_

#include <memory>

#include <Box2D/Box2D.h>

namespace vigilante {

class b2BodyBuilder {
 public:
  explicit b2BodyBuilder(b2World* world);
  explicit b2BodyBuilder(b2Body* body);
  explicit b2BodyBuilder(const b2BodyBuilder& bodyBuilder);
  b2BodyBuilder& operator=(const b2BodyBuilder& bodyBuilder);
  virtual ~b2BodyBuilder() = default;

  b2BodyBuilder& type(b2BodyType bodyType);
  b2BodyBuilder& position(float x, float y, float ppm);
  b2BodyBuilder& position(b2Vec2 position, float ppm);
  b2Body* buildBody();

  b2BodyBuilder& newRectangleFixture(float hx, float hy, float ppm);
  b2BodyBuilder& newPolygonFixture(const b2Vec2* vertices, size_t count, float ppm);
  b2BodyBuilder& newPolylineFixture(const b2Vec2* vertices, size_t count, float ppm);
  b2BodyBuilder& newEdgeShapeFixture(const b2Vec2& vertex1, const b2Vec2& vertex2, float ppm);
  b2BodyBuilder& newCircleFixture(const b2Vec2& centerPos, int radius, float ppm);

  b2BodyBuilder& categoryBits(short categoryBits);
  b2BodyBuilder& maskBits(short maskBits);
  b2BodyBuilder& setSensor(bool isSensor);
  b2BodyBuilder& friction(float friction);
  b2BodyBuilder& restitution(float restitution);
  b2BodyBuilder& setUserData(void* userData);
  b2Fixture* buildFixture();

 private:
  b2World* _world;
  b2Body* _body;
  b2Fixture* _fixture;

  b2BodyDef _bdef;
  b2FixtureDef _fdef;
  std::unique_ptr<b2Shape> _shape;
  void* _userData;
};

}  // namespace vigilante

#endif  // VIGILANTE_B2_BODY_BUILDER_H_
