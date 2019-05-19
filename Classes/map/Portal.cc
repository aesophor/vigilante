#include "Portal.h"

using std::string;

namespace vigilante {

Portal::Portal(const string& targetTmxMapFileName, int targetPortalId, b2Body* body)
    : targetTmxMapFileName(targetTmxMapFileName),
      targetPortalId(targetPortalId),
      body(body) {}

} // namespace vigilante
