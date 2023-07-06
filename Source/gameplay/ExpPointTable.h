// Copyright (c) 2018-2021 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_EXP_POINT_TABLE_H_
#define VIGILANTE_EXP_POINT_TABLE_H_

#include <string>

namespace vigilante::exp_point_table {

inline constexpr int kLevelCap = 100;

void import(const std::string& tableFileName);
int getNextLevelExp(const int currentLevel);

}  // namespace vigilante::exp_point_table

#endif  // VIGILANTE_EXP_POINT_TABLE_H_
