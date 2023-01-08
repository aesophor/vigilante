// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_KEY_H_
#define VIGILANTE_KEY_H_

#include <string>

#include "MiscItem.h"

namespace vigilante {

class Key : public MiscItem {
 public:
  struct Profile final {
    explicit Profile(const std::string& jsonFileName);

    std::string targetTmxFileName;
    int targetPortalId;
  };

  explicit Key(const std::string& jsonFileName);
  virtual ~Key() override = default;

  inline const Key::Profile& getKeyProfile() const { return _keyProfile; }

 private:
  Key::Profile _keyProfile;
};

}  // namespace vigilante

#endif  // VIGILANTE_KEY_H_
