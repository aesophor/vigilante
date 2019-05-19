#ifndef VIGILANTE_PORTAL_H_
#define VIGILANTE_PORTAL_H_

#include <string>

#include "Box2D/Box2D.h"

namespace vigilante {

struct Portal {
  Portal(const std::string& targetTmxMapFileName, int targetPortalId, b2Body* body);
  std::string targetTmxMapFileName;
  int targetPortalId;
  b2Body* body;
};

} // namespace vigilante

#endif // VIGILANTE_PORTAL_H_
