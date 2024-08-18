// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_ITEM_KEY_H_
#define VIGILANTE_ITEM_KEY_H_

#include <string>

#include "MiscItem.h"

namespace vigilante {

class Key : public MiscItem {
 public:
  struct Profile final {
    explicit Profile(const std::filesystem::path& jsonFilePath);

    std::filesystem::path targetTmxFilePath;
    int targetPortalId;
  };

  explicit Key(const std::filesystem::path& jsonFilePath);
  virtual ~Key() override = default;

  inline const Key::Profile& getKeyProfile() const { return _keyProfile; }

 private:
  Key::Profile _keyProfile;
};

}  // namespace vigilante

#endif  // VIGILANTE_ITEM_KEY_H_
