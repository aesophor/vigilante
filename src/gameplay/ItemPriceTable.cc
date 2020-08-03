// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "ItemPriceTable.h"

#include <fstream>
#include <unordered_map>
#include <stdexcept>

#include "util/Logger.h"

using std::string;
using std::unordered_map;
using std::ifstream;
using std::runtime_error;

namespace vigilante {

namespace {

unordered_map<string, int> prices;

}  // namespace
  
namespace item_price_table {

void import(const string& tableFileName) {
  ifstream fin(tableFileName);
  if (!fin.is_open()) {
    throw runtime_error("Failed to import item price table from: " + tableFileName);
  }

  VGLOG(LOG_INFO, "Loading item price table...");
  while (fin) {
    string itemJsonFileName;
    int price;
    fin >> itemJsonFileName >> price;
    prices[itemJsonFileName] = price;
  }
}

int getPrice(Item* item) {
  return prices.at(item->getItemProfile().jsonFileName);
}

}  // namespace item_price_table

}  // namespace vigilante
