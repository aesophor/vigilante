// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "b2BodyBuilder.h"

using namespace std;

namespace vigilante {

b2BodyBuilder::b2BodyBuilder(b2World* world)
    : _world(world), _body(), _fixture(), _shape(), _userData() {}

b2BodyBuilder::b2BodyBuilder(b2Body* body)
    : _world(body->GetWorld()), _body(body), _fixture(), _shape(), _userData() {}

b2BodyBuilder::b2BodyBuilder(const b2BodyBuilder& bodyBuilder)
    : _world(bodyBuilder._world),
      _body(bodyBuilder._body),
      _fixture(bodyBuilder._fixture),
      _shape(bodyBuilder._shape.get()),
      _userData(bodyBuilder._userData) {}

b2BodyBuilder& b2BodyBuilder::operator= (const b2BodyBuilder& bodyBuilder) {
  _world = bodyBuilder._world;
  _body = bodyBuilder._body;
  _fixture = bodyBuilder._fixture;
  _shape = unique_ptr<b2Shape>(bodyBuilder._shape.get());
  _userData = bodyBuilder._userData;
  return *this;
}

b2BodyBuilder& b2BodyBuilder::type(b2BodyType bodyType) {
  _bdef.type = bodyType;
  _bdef.fixedRotation = true;
  return *this;
}

b2BodyBuilder& b2BodyBuilder::position(float x, float y, float ppm) {
  _bdef.position.Set(x / ppm, y / ppm);
  return *this;
}

b2Body* b2BodyBuilder::buildBody() {
  _body = _world->CreateBody(&_bdef);
  return _body;
}


b2BodyBuilder& b2BodyBuilder::newRectangleFixture(float hw, float hh, float ppm) {
  _shape = std::make_unique<b2PolygonShape>();
  _fdef.shape = _shape.get();

  b2PolygonShape* shape = static_cast<b2PolygonShape*>(_shape.get());
  shape->SetAsBox(hw / ppm, hh / ppm);
  return *this;
}

b2BodyBuilder& b2BodyBuilder::newPolygonFixture(const b2Vec2* vertices, size_t count, float ppm) {
  _shape = std::make_unique<b2PolygonShape>();
  _fdef.shape = _shape.get();

  b2PolygonShape* shape = static_cast<b2PolygonShape*>(_shape.get());
  b2Vec2 scaledVertices[count];
  for (size_t i = 0; i < count; i++) {
    scaledVertices[i] = {vertices[i].x / ppm, vertices[i].y / ppm};
  }
  shape->Set(scaledVertices, count);
  return *this;
}

b2BodyBuilder& b2BodyBuilder::newPolylineFixture(const b2Vec2* vertices, size_t count, float ppm) {
  /*
  _shape = std::make_unique<b2ChainShape>();
  _fdef.shape = _shape.get();

  b2ChainShape* shape = static_cast<b2ChainShape*>(_shape.get());
  b2Vec2 scaledVertices[count];
  for (size_t i = 0; i < count; i++) {
    scaledVertices[i] = {vertices[i].x / ppm, vertices[i].y / ppm};
  }
  
  b2Vec2 prevVertex, nextVertex;
  // provide your own logic to decide the prevVertex and nextVertex
  // e.g., if you just want to use the first and last vertices:
  prevVertex = scaledVertices[count - 1];
  nextVertex = scaledVertices[0];
  
  shape->CreateChain(scaledVertices, count, prevVertex, nextVertex);
  return *this;
  */
  return newEdgeShapeFixture(vertices[0], vertices[1], ppm);
}

b2BodyBuilder& b2BodyBuilder::newEdgeShapeFixture(const b2Vec2& vertex1, const b2Vec2& vertex2, float ppm) {
  _shape = std::make_unique<b2EdgeShape>();
  _fdef.shape = _shape.get();

  b2EdgeShape* shape = static_cast<b2EdgeShape*>(_shape.get());
  b2Vec2 scaledVertex1 = {vertex1.x / ppm, vertex1.y / ppm};
  b2Vec2 scaledVertex2 = {vertex2.x / ppm, vertex2.y / ppm};
  shape->SetTwoSided(scaledVertex1, scaledVertex2);
  return *this;
}

b2BodyBuilder& b2BodyBuilder::newCircleFixture(const b2Vec2& centerPos, int radius, float ppm) {
  _shape = std::make_unique<b2CircleShape>();
  _fdef.shape = _shape.get();

  b2CircleShape* shape = static_cast<b2CircleShape*>(_shape.get());
  shape->m_p.Set(centerPos.x / ppm, centerPos.y / ppm);
  shape->m_radius = radius / ppm;
  return *this;
}


b2BodyBuilder& b2BodyBuilder::categoryBits(short categoryBits) {
  _fdef.filter.categoryBits = categoryBits;
  return *this;
}

b2BodyBuilder& b2BodyBuilder::maskBits(short maskBits) {
  _fdef.filter.maskBits = maskBits;
  return *this;
}

b2BodyBuilder& b2BodyBuilder::setSensor(bool isSensor) {
  _fdef.isSensor = isSensor;
  return *this;
}

b2BodyBuilder& b2BodyBuilder::friction(float friction) {
  _fdef.friction = friction;
  return *this;
}

b2BodyBuilder& b2BodyBuilder::density(float density) {
  _fdef.density = density;
  return *this;
}

b2BodyBuilder& b2BodyBuilder::restitution(float restitution) {
  _fdef.restitution = restitution;
  return *this;
}

b2BodyBuilder& b2BodyBuilder::setUserData(void* userData) {
  _userData = userData;
  return *this;
}

b2Fixture* b2BodyBuilder::buildFixture() {
  _fixture = _body->CreateFixture(&_fdef);
  _fixture->GetUserData().pointer = reinterpret_cast<uintptr_t>(_userData);
  _fdef = b2FixtureDef{}; // Clear _fdef data
  return _fixture;
}

}  // namespace vigilante
