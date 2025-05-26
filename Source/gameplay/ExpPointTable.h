// Copyright (c) 2018-2024 Marco Wang <m.aesophor@gmail.com>. All rights reserved.
#ifndef REQUIEM_GAMEPLAY_EXP_POINT_TABLE_H_
#define REQUIEM_GAMEPLAY_EXP_POINT_TABLE_H_

#include <filesystem>

namespace requiem::exp_point_table {

inline constexpr int kLevelCap = 100;

void import(const std::filesystem::path& tableFilePath);
int getNextLevelExp(const int currentLevel);

}  // namespace requiem::exp_point_table

#endif  // REQUIEM_GAMEPLAY_EXP_POINT_TABLE_H_
