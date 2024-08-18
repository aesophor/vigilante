// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.

#include "ExpPointTable.h"

#include <array>
#include <fstream>
#include <stdexcept>

#include "util/Logger.h"

namespace fs = std::filesystem;
using namespace std;

namespace vigilante {

namespace {

array<int, exp_point_table::kLevelCap> levelUpExp;

}  // namespace

namespace exp_point_table {

void import(const fs::path& tableFilePath) {
  ifstream fin{tableFilePath};
  if (!fin.is_open()) {
    throw runtime_error("Failed to import exp point table from: " + tableFilePath.native());
  }

  VGLOG(LOG_INFO, "Loading exp point table...");
  for (int i = 0; i < kLevelCap - 1; i++) {
    int level;
    int exp;
    fin >> level >> exp;
    levelUpExp[level] = exp;
  }
}

int getNextLevelExp(const int currentLevel) {
  return levelUpExp[currentLevel];
}

}  // namespace exp_point_table

}  // namespace vigilante
