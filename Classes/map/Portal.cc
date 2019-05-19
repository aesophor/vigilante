#include "Portal.h"

using std::string;

namespace vigilante {

Portal::Portal(const string& targetTmxMapFileName, int targetPortalId)
    : _targetTmxMapFileName(targetTmxMapFileName), _targetPortalId(targetPortalId) {}


const string& Portal::getTargetTmxMapFileName() const {
  return _targetTmxMapFileName;
}

int Portal::getTargetPortalId() const {
  return _targetPortalId;
}

} // namespace vigilante
