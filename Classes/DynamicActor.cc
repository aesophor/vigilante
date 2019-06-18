#include "DynamicActor.h"

#include "Box2D/Box2D.h"

#include "Constants.h"
#include "map/GameMapManager.h"

using std::vector;

namespace vigilante {

DynamicActor::DynamicActor(size_t numAnimations, size_t numFixtures)
    : StaticActor(numAnimations),
      _body(),
      _fixtures(numFixtures) {}


void DynamicActor::removeFromMap() {
  if (_isShownOnMap) {
    _body->GetWorld()->DestroyBody(_body);
    GameMapManager::getInstance()->getLayer()->removeChild(_bodySprite);
    _bodySprite = nullptr;
    _isShownOnMap = false;
  }
}

void DynamicActor::update(float delta) {
  // Sync the body sprite with its b2body.
  b2Vec2 b2bodyPos = _body->GetPosition();
  _bodySprite->setPosition(b2bodyPos.x * kPpm, b2bodyPos.y * kPpm);
}


b2Body* DynamicActor::getBody() const {
  return _body;
}

vector<b2Fixture*>& DynamicActor::getFixtures() {
  return _fixtures;
}

} // namespace vigilante
