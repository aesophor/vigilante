// Copyright (c) 2018-2020 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#include "ExpPointTable.h"

#include <array>
#include <fstream>
#include <stdexcept>

#include <cocos2d.h>
#include "util/Logger.h"

using std::string;
using std::array;
using std::ifstream;
using std::runtime_error;

namespace vigilante {

namespace {

array<int, exp_point_table::levelCap> levelUpExp;

} // namespace


namespace exp_point_table {

void import(const string& tableFileName) {
  ifstream fin(tableFileName);
  if (!fin.is_open()) {
    throw runtime_error("Failed to import exp point table from: " + tableFileName);
  }

  VGLOG(LOG_INFO, "Loading exp point table...");
  for (int i = 0; i < levelCap - 1; i++) {
    int level;
    int exp;
    fin >> level >> exp;
    levelUpExp[level] = exp;
  }
}

int getNextLevelExp(int currentLevel) {
  return levelUpExp[currentLevel];
}

} // namespace exp_point_table

} // namespace vigilante
