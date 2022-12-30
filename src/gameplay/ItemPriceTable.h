// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_ITEM_PRICE_TABLE_H_
#define VIGILANTE_ITEM_PRICE_TABLE_H_

#include <string>

#include "item/Item.h"

namespace vigilante::item_price_table {

void import(const std::string& tableFileName);
int getPrice(Item* item);

}  // namespace vigilante::item_price_table

#endif  // VIGILANTE_ITEM_PRICE_TABLE_H_
