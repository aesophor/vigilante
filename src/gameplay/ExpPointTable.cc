// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "ExpPointTable.h"

#include <array>
#include <fstream>
#include <stdexcept>

#include "util/Logger.h"

using std::string;
using std::array;
using std::ifstream;
using std::runtime_error;

namespace vigilante {

namespace {

array<int, exp_point_table::kLevelCap> levelUpExp;

}  // namespace


namespace exp_point_table {

void import(const string& tableFileName) {
  ifstream fin(tableFileName);
  if (!fin.is_open()) {
    throw runtime_error("Failed to import exp point table from: " + tableFileName);
  }

  VGLOG(LOG_INFO, "Loading exp point table...");
  for (int i = 0; i < kLevelCap - 1; i++) {
    int level;
    int exp;
    fin >> level >> exp;
    levelUpExp[level] = exp;
  }
}

int getNextLevelExp(int currentLevel) {
  return levelUpExp[currentLevel];
}

}  // namespace exp_point_table

}  // namespace vigilante
