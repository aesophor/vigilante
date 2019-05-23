#include "DynamicActor.h"

namespace vigilante {

DynamicActor::DynamicActor(size_t animationsSize, size_t fixturesSize)
    : StaticActor(animationsSize),
      _body(),
      _fixtures(fixturesSize) {}


b2Body* DynamicActor::getBody() const {
  return _body;
}

} // namespace vigilante
