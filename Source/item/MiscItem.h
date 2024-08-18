// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef VIGILANTE_ITEM_MISC_ITEM_H_
#define VIGILANTE_ITEM_MISC_ITEM_H_

#include <string>

#include "Item.h"

namespace vigilante {

class MiscItem : public Item {
 public:
  explicit MiscItem(const std::filesystem::path& jsonFilePath);
};

}  // namespace vigilante

#endif  // VIGILANTE_ITEM_MISC_ITEM_H_
