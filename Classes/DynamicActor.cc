#include "DynamicActor.h"

#include "map/GameMapManager.h"

using std::vector;

namespace vigilante {

DynamicActor::DynamicActor(size_t animationsSize, size_t fixturesSize)
    : StaticActor(animationsSize),
      _body(),
      _fixtures(fixturesSize) {}


void DynamicActor::removeFromMap() {
  if (_isShownOnMap) {
    _body->GetWorld()->DestroyBody(_body);
    GameMapManager::getInstance()->getLayer()->removeChild(_bodySprite);

    _isShownOnMap = false;
  }
}

b2Body* DynamicActor::getBody() const {
  return _body;
}

vector<b2Fixture*>& DynamicActor::getFixtures() {
  return _fixtures;
}

} // namespace vigilante
