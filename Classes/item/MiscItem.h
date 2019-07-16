// Copyright (c) 2019 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_MISC_ITEM_H_
#define VIGILANTE_MISC_ITEM_H_

#include <string>

#include "Item.h"

namespace vigilante {

class MiscItem : public Item {
 public:
  MiscItem(const std::string& jsonFileName);
  virtual ~MiscItem() = default;
};

} // namespace vigilante

#endif // VIGILANTE_MISC_ITEM_H_
