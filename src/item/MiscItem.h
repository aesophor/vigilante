// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_MISC_ITEM_H_
#define VIGILANTE_MISC_ITEM_H_

#include <string>

#include "Item.h"

namespace vigilante {

class MiscItem : public Item {
 public:
  explicit MiscItem(const std::string& jsonFileName);
  virtual ~MiscItem() = default;
};

}  // namespace vigilante

#endif  // VIGILANTE_MISC_ITEM_H_
