#ifndef VIGILANTE_PORTAL_H_
#define VIGILANTE_PORTAL_H_

#include <string>

#include "Box2D/Box2D.h"

namespace vigilante {

class Portal {
 public:
  Portal(const std::string& targetTmxMapFileName, int targetPortalId, b2Body* body);
  virtual ~Portal() = default;

  const std::string& getTargetTmxMapFileName() const;
  int getTargetPortalId() const;
  b2Body* getBody() const;

 private:
  std::string _targetTmxMapFileName;
  int _targetPortalId;
  b2Body* _body;
};

} // namespace vigilante

#endif // VIGILANTE_PORTAL_H_
