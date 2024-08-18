// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef VIGILANTE_GAMEPLAY_EXP_POINT_TABLE_H_
#define VIGILANTE_GAMEPLAY_EXP_POINT_TABLE_H_

#include <filesystem>

namespace vigilante::exp_point_table {

inline constexpr int kLevelCap = 100;

void import(const std::filesystem::path& tableFilePath);
int getNextLevelExp(const int currentLevel);

}  // namespace vigilante::exp_point_table

#endif  // VIGILANTE_GAMEPLAY_EXP_POINT_TABLE_H_
