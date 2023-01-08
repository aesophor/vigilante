// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "ItemPriceTable.h"

#include <fstream>
#include <unordered_map>
#include <stdexcept>

#include "util/Logger.h"

using namespace std;

namespace {

unordered_map<string, int> prices;

}  // namespace

namespace vigilante::item_price_table {

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
    if (!itemJsonFileName.empty()) {
      prices[itemJsonFileName] = price;
    }
  }
}

int getPrice(Item* item) {
  const string& itemJsonFileName = item->getItemProfile().jsonFileName;
  auto it = prices.find(itemJsonFileName);
  if (it == prices.end()) {
    VGLOG(LOG_ERR, "Failed to find the price of [%s].", itemJsonFileName.c_str());
    return 0;
  }

  return it->second;
}

}  // namespace vigilante::item_price_table
