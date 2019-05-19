#ifndef VIGILANTE_PORTAL_H_
#define VIGILANTE_PORTAL_H_

#include <string>

namespace vigilante {

class Portal {
 public:
  Portal(const std::string& targetTmxMapFileName, int targetPortalId);
  virtual ~Portal() = default;

  const std::string& getTargetTmxMapFileName() const;
  int getTargetPortalId() const;

 private:
  std::string _targetTmxMapFileName;
  int _targetPortalId;
};

} // namespace vigilante

#endif // VIGILANTE_PORTAL_H_
