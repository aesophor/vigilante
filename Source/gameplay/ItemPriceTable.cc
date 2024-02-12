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

void import(const string& tableFilePath) {
  ifstream fin{tableFilePath};
  if (!fin.is_open()) {
    throw runtime_error("Failed to import item price table from: " + tableFilePath);
  }

  VGLOG(LOG_INFO, "Loading item price table...");
  while (fin) {
    string itemJsonFilePath;
    int price;
    fin >> itemJsonFilePath >> price;
    if (!itemJsonFilePath.empty()) {
      prices[itemJsonFilePath] = price;
    }
  }
}

int getPrice(const Item* item) {
  const string& itemJsonFilePath = item->getItemProfile().jsonFilePath;
  auto it = prices.find(itemJsonFilePath);
  if (it == prices.end()) {
    VGLOG(LOG_ERR, "Failed to find the price of [%s].", itemJsonFilePath.c_str());
    return 0;
  }

  return it->second;
}

}  // namespace vigilante::item_price_table
