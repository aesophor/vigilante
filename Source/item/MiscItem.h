// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#ifndef REQUIEM_ITEM_MISC_ITEM_H_
#define REQUIEM_ITEM_MISC_ITEM_H_

#include <string>

#include "Item.h"

namespace requiem {

class MiscItem : public Item {
 public:
  explicit MiscItem(const std::filesystem::path& jsonFilePath);
};

}  // namespace requiem

#endif  // REQUIEM_ITEM_MISC_ITEM_H_
