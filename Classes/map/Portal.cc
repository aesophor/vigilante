#include "Portal.h"

using std::string;

namespace vigilante {

Portal::Portal(const string& targetTmxMapFileName, int targetPortalId, b2Body* body)
    : _targetTmxMapFileName(targetTmxMapFileName),
      _targetPortalId(targetPortalId),
      _body(body) {}


const string& Portal::getTargetTmxMapFileName() const {
  return _targetTmxMapFileName;
}

int Portal::getTargetPortalId() const {
  return _targetPortalId;
}

b2Body* Portal::getBody() const {
  return _body;
}

} // namespace vigilante
